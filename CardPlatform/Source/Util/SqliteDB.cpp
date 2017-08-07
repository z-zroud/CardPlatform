#include "StdAfx.h"
#include "SqliteDB.h"

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