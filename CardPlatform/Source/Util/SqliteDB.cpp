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

BOOL SQLite::Open(LPCTSTR lpDbFlie)
{
    if (lpDbFlie == NULL)
    {
        return FALSE;
    } 
    if (sqlite3_open(lpDbFlie, &m_db) != SQLITE_OK) 
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

/***************************************************
* 功能：打开数据库
****************************************************/
bool SqliteDB::OpenDB(string dbFile)
{
    sqlite3 *db = NULL;
    char *errmsg = NULL;

    //打开数据库
    int ret = sqlite3_open(dbFile.c_str(), &m_db);
    if (ret != SQLITE_OK)
    {
        CloseDb();
        return false;
    }

    return true;
}

/***************************************************
* 功能：关闭数据库
****************************************************/
void SqliteDB::CloseDb()
{
    if (m_db)
    {
        sqlite3_close(m_db);
    }
}

bool SqliteDB::QueryCol(string table, string colName, vector<string> &vecResult)
{
    CALLBACK_PARAM params;
    params.colName = colName;
    string sql = _T("select ") + colName + _T(" from ") + table;

    if (!Exec(sql, QueryResult, &params))
        return false;

    for (auto v : params.colValues)
    {
        vecResult.push_back(v);
    }

    return true;
}

string SqliteDB::Query(string table, string colName, string uniqKey)
{
    return _T("");
}
bool SqliteDB::Insert(string table, string insertSql)
{
    //insert into KMC (kmcValue) values ("111")
    return false;
}
bool SqliteDB::Delete(string table, string uniqKey)
{
    //DELETE FROM 表名称 WHERE 列名称 = 值
    return false;
}

/*******************************************************************************
* 功能：处理查询结果中的每条记录
********************************************************************************/
int SqliteDB::QueryResult(void *params, int columnNum, char **columnVal, char **columnName)
{
    CALLBACK_PARAM* pParams = static_cast<CALLBACK_PARAM*>(params);
    for (int i = 0; i < columnNum; i++)
    {
        if (string(columnName[i]) == pParams->colName)
        {
            pParams->colValues.push_back(columnVal[i]);
            break;
        }
    }

    return 0;
}

/***********************************************************************************
* 功能： 执行SQL语句，返回的每一条记录由calbackFunc回调函数进行处理，param为传递给回调函数
* 的参数
************************************************************************************/
bool SqliteDB::Exec(string sql, sqlite3_callback callbackFunc, CALLBACK_PARAM* param)
{
    char *errmsg = NULL;
    int ret = sqlite3_exec(m_db, sql.c_str(), callbackFunc, (void*)param, &errmsg);
    if (ret != SQLITE_OK)
    {
        sqlite3_free(errmsg);
        return false;
    }

    return true;
}

DB_KMC::DB_KMC()
{
    m_db.OpenDB(_T("./Configuration/pcsc.db"));
    m_table = _T("KMC");
}

bool DB_KMC::GetKmc(vector<string> &vecResult)
{
    return m_db.QueryCol(m_table, _T("kmcValue"), vecResult);
}

bool DB_KMC::AddKmc(string kmc)
{
    return false;
}
void DB_KMC::DeleteKmc(string kmc)
{

}

DB_AID::DB_AID()
{
    m_db.OpenDB(_T("./Configuration/pcsc.db"));
    m_table = _T("AID");
}

bool DB_AID::GetAid(vector<string> &vecResult)
{
    return m_db.QueryCol(m_table, _T("aidValue"), vecResult);
}

bool DB_AID::AddAid(string aid)
{
    return false;
}

void DB_AID::DeleteAid(string aid)
{
    return ;
}