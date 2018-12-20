#include "StdAfx.h"
#include "SqliteDB.h"


SQLite::SQLite(void) :
    m_db(NULL)
{
}

SQLite::~SQLite(void)
{
    Close();
}

wstring AsciiToUnicode(const string& str) {
    // 预算-缓冲区中宽字节的长度    
    int unicodeLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    // 给指向缓冲区的指针变量分配内存    
    wchar_t *pUnicode = (wchar_t*)malloc(sizeof(wchar_t)*unicodeLen);
    // 开始向缓冲区转换字节    
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, pUnicode, unicodeLen);
    wstring ret_str = pUnicode;
    free(pUnicode);
    return ret_str;
}

BOOL SQLite::Open(LPCTSTR lpDbFlie)
{
    if (lpDbFlie == NULL)
    {
        return FALSE;
    } 
    wstring dpPath = AsciiToUnicode(lpDbFlie);
    if (sqlite3_open16(dpPath.c_str(), &m_db) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

void SQLite::Close()
{
    if (m_db)
    {
        sqlite3_close(m_db);
        m_db = NULL;
    }
}

BOOL SQLite::ExcuteNonQuery(LPCTSTR lpSql)
{
    if (lpSql == NULL)
    {
        return FALSE;
    }
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, lpSql, -1, &stmt, NULL) != SQLITE_OK)  
    {
        return FALSE;
    }
    sqlite3_step(stmt);

    return (sqlite3_finalize(stmt) == SQLITE_OK) ? TRUE : FALSE;
}

BOOL SQLite::ExcuteNonQuery(SQLiteCommand* pCmd)
{
    if (pCmd == NULL)
    {
        return FALSE;
    }
    return pCmd->Excute();
}

// 查询(回调方式)   
BOOL SQLite::ExcuteQuery(LPCTSTR lpSql, sqlite3_callback pCallBack)
{
    if (lpSql == NULL || pCallBack == NULL)
    {
        return FALSE;
    }
    char *errmsg = NULL;
    if (sqlite3_exec(m_db, lpSql, pCallBack, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }  
    return TRUE;
}

// 查询   
SQLiteDataReader SQLite::ExcuteQuery(LPCTSTR lpSql)
{
    if (lpSql == NULL)
    {
        return FALSE;
    }
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, lpSql, -1, &stmt, NULL) != SQLITE_OK)  
    {
        return FALSE;
    }
    return SQLiteDataReader(stmt);
}

// 开始事务   
BOOL SQLite::BeginTransaction()
{
    char * errmsg = NULL;
    if (sqlite3_exec(m_db, "BEGIN TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

// 提交事务   
BOOL SQLite::CommitTransaction()
{
    char * errmsg = NULL;
    if (sqlite3_exec(m_db, "COMMIT TRANSACTION;;", NULL, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

// 回滚事务   
BOOL SQLite::RollbackTransaction()
{
    char * errmsg = NULL;
    if (sqlite3_exec(m_db, "ROLLBACK  TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

// 获取上一条错误信息   
LPCTSTR SQLite::GetLastErrorMsg()
{
    return sqlite3_errmsg(m_db);
}



SQLiteDataReader::SQLiteDataReader(sqlite3_stmt *pStmt) :
    m_pStmt(pStmt)
{

}

SQLiteDataReader::~SQLiteDataReader()
{
    Close();
}

// 读取一行数据   
BOOL SQLiteDataReader::Read()
{
    if (m_pStmt == NULL)
    {
        return FALSE;
    }
    if (sqlite3_step(m_pStmt) != SQLITE_ROW)
    {
        return FALSE;
    }
    return TRUE;
}

// 关闭Reader，读取结束后调用   
void SQLiteDataReader::Close()
{
    if (m_pStmt)
    {
        sqlite3_finalize(m_pStmt);
        m_pStmt = NULL;
    }
}

// 总的列数   
int SQLiteDataReader::ColumnCount(void)
{
    return sqlite3_column_count(m_pStmt);
}

// 获取某列的名称    
LPCTSTR SQLiteDataReader::GetColumnName(int nCol)
{
    return (LPCTSTR)sqlite3_column_name(m_pStmt, nCol);
}

// 获取某列的数据类型   
SQLITE_DATATYPE SQLiteDataReader::GetDataType(int nCol)
{
    return (SQLITE_DATATYPE)sqlite3_column_type(m_pStmt, nCol);
}

// 获取某列的值(字符串)   
LPCTSTR SQLiteDataReader::GetStringValue(int nCol)
{
#ifdef  UNICODE    
    return (LPCTSTR)sqlite3_column_text16(m_pStmt, nCol);
#else   
    return (LPCTSTR)sqlite3_column_text(m_pStmt, nCol);
#endif   
}

// 获取某列的值(整形)   
int SQLiteDataReader::GetIntValue(int nCol)
{
    return sqlite3_column_int(m_pStmt, nCol);
}

// 获取某列的值(长整形)   
long SQLiteDataReader::GetInt64Value(int nCol)
{
    return (long)sqlite3_column_int64(m_pStmt, nCol);
}

// 获取某列的值(浮点形)   
double SQLiteDataReader::GetFloatValue(int nCol)
{
    return sqlite3_column_double(m_pStmt, nCol);
}

// 获取某列的值(二进制数据)   
const BYTE* SQLiteDataReader::GetBlobValue(int nCol, int &nLen)
{
    nLen = sqlite3_column_bytes(m_pStmt, nCol);
    return (const BYTE*)sqlite3_column_blob(m_pStmt, nCol);
}

SQLiteCommand::SQLiteCommand(SQLite* pSqlite) :
    m_pSqlite(pSqlite),
    m_pStmt(NULL)
{
}

SQLiteCommand::SQLiteCommand(SQLite* pSqlite, LPCTSTR lpSql) :
    m_pSqlite(pSqlite),
    m_pStmt(NULL)
{
    SetCommandText(lpSql);
}

SQLiteCommand::~SQLiteCommand()
{

}

BOOL SQLiteCommand::SetCommandText(LPCTSTR lpSql)
{  
    if (sqlite3_prepare_v2(m_pSqlite->m_db, lpSql, -1, &m_pStmt, NULL) != SQLITE_OK)  
    {
        return FALSE;
    }
    return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, LPCTSTR szValue)
{
    if (sqlite3_bind_text(m_pStmt, index, szValue, -1, SQLITE_TRANSIENT) != SQLITE_OK) 
    {
        return FALSE;
    }
    return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, const int nValue)
{
    if (sqlite3_bind_int(m_pStmt, index, nValue) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, const double dValue)
{
    if (sqlite3_bind_double(m_pStmt, index, dValue) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL SQLiteCommand::BindParam(int index, const unsigned char* blobBuf, int nLen)
{
    if (sqlite3_bind_blob(m_pStmt, index, blobBuf, nLen, NULL) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL SQLiteCommand::Excute()
{
    sqlite3_step(m_pStmt);

    return (sqlite3_reset(m_pStmt) == SQLITE_OK) ? TRUE : FALSE;
}

void SQLiteCommand::Clear()
{
    if (m_pStmt)
    {
        sqlite3_finalize(m_pStmt);
    }
}

/**********************************************************************************/

DB_KMC::DB_KMC()
{
    m_db.Open("./Configuration/pcsc.db");
}

DB_KMC::~DB_KMC()
{
    m_db.Close();
}

bool DB_KMC::GetKmc(vector<string> &vecResult)
{
    SQLiteDataReader dbReader = m_db.ExcuteQuery("select * from kmc;");
    
    while (dbReader.Read())
    {
        vecResult.push_back(dbReader.GetStringValue(1));
    }

    return true;
}

DB_AID::DB_AID()
{
    m_db.Open("./Configuration/pcsc.db");
}

DB_AID::~DB_AID()
{
    m_db.Close();
}

bool DB_AID::GetAid(vector<string> &vecResult)
{
    SQLiteDataReader dbReader = m_db.ExcuteQuery("select * from aid;");

    while (dbReader.Read())
    {
        vecResult.push_back(dbReader.GetStringValue(1));
    }

    return true;
}