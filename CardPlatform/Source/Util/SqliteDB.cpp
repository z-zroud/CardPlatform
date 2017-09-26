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

// ��ѯ(�ص���ʽ)   
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

// ��ѯ   
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

// ��ʼ����   
BOOL SQLite::BeginTransaction()
{
    char * errmsg = NULL;
    if (sqlite3_exec(m_db, "BEGIN TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

// �ύ����   
BOOL SQLite::CommitTransaction()
{
    char * errmsg = NULL;
    if (sqlite3_exec(m_db, "COMMIT TRANSACTION;;", NULL, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

// �ع�����   
BOOL SQLite::RollbackTransaction()
{
    char * errmsg = NULL;
    if (sqlite3_exec(m_db, "ROLLBACK  TRANSACTION;", NULL, NULL, &errmsg) != SQLITE_OK)
    {
        return FALSE;
    }
    return TRUE;
}

// ��ȡ��һ��������Ϣ   
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

// ��ȡһ������   
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

// �ر�Reader����ȡ���������   
void SQLiteDataReader::Close()
{
    if (m_pStmt)
    {
        sqlite3_finalize(m_pStmt);
        m_pStmt = NULL;
    }
}

// �ܵ�����   
int SQLiteDataReader::ColumnCount(void)
{
    return sqlite3_column_count(m_pStmt);
}

// ��ȡĳ�е�����    
LPCTSTR SQLiteDataReader::GetColumnName(int nCol)
{
    return (LPCTSTR)sqlite3_column_name(m_pStmt, nCol);
}

// ��ȡĳ�е���������   
SQLITE_DATATYPE SQLiteDataReader::GetDataType(int nCol)
{
    return (SQLITE_DATATYPE)sqlite3_column_type(m_pStmt, nCol);
}

// ��ȡĳ�е�ֵ(�ַ���)   
LPCTSTR SQLiteDataReader::GetStringValue(int nCol)
{
#ifdef  UNICODE    
    return (LPCTSTR)sqlite3_column_text16(m_pStmt, nCol);
#else   
    return (LPCTSTR)sqlite3_column_text(m_pStmt, nCol);
#endif   
}

// ��ȡĳ�е�ֵ(����)   
int SQLiteDataReader::GetIntValue(int nCol)
{
    return sqlite3_column_int(m_pStmt, nCol);
}

// ��ȡĳ�е�ֵ(������)   
long SQLiteDataReader::GetInt64Value(int nCol)
{
    return (long)sqlite3_column_int64(m_pStmt, nCol);
}

// ��ȡĳ�е�ֵ(������)   
double SQLiteDataReader::GetFloatValue(int nCol)
{
    return sqlite3_column_double(m_pStmt, nCol);
}

// ��ȡĳ�е�ֵ(����������)   
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
* ���ܣ������ݿ�
****************************************************/
bool SqliteDB::OpenDB(string dbFile)
{
    sqlite3 *db = NULL;
    char *errmsg = NULL;

    //�����ݿ�
    int ret = sqlite3_open(dbFile.c_str(), &m_db);
    if (ret != SQLITE_OK)
    {
        CloseDb();
        return false;
    }

    return true;
}

/***************************************************
* ���ܣ��ر����ݿ�
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
    //DELETE FROM ������ WHERE ������ = ֵ
    return false;
}

/*******************************************************************************
* ���ܣ������ѯ����е�ÿ����¼
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
* ���ܣ� ִ��SQL��䣬���ص�ÿһ����¼��calbackFunc�ص��������д���paramΪ���ݸ��ص�����
* �Ĳ���
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