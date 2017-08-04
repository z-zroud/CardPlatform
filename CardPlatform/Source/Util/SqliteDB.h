#pragma once
#include <string>
#include <vector>
#include "Sqlite3\sqlite3.h"
using namespace std;

struct CALLBACK_PARAM
{
    string colName;
    vector<string> colValues;
};

/**************************************************************
* SqliteDB 简单的封装了sql的增删查功能，主要存储CardPlatform相关的
* 配置信息
***************************************************************/
class SqliteDB
{
public:
    bool OpenDB(string dbTable);
    bool QueryCol( string sql, string colName, vector<string> &vecResult);
    string Query(string sql, string colName, string uniqKey);
    bool Insert(string sql, string insertSql);
    bool Delete(string sql, string uniqKey);
    void CloseDb();
protected:
    bool Exec(string sql, sqlite3_callback callbackFunc, CALLBACK_PARAM* param);
    static int QueryResult(void *params, int columnNum, char **columnVal, char **columnName);
private:
    string m_tableName;
    sqlite3*    m_db;
};

/***********************************************************
* 功能：用于操作KMC表
************************************************************/
class DB_KMC
{
public:
    DB_KMC();
    bool GetKmc(vector<string> &vecResult);
    bool AddKmc(string kmc);
    void DeleteKmc(string kmc);

private:
    SqliteDB    m_db;
    string      m_table;
};

/***********************************************************
* 功能：用于操作AID表
************************************************************/
class DB_AID
{
public:
    DB_AID();
    bool GetAid(vector<string> &vecResult);
    bool AddAid(string aid);
    void DeleteAid(string aid);

private:
    SqliteDB    m_db;
    string      m_table;
};