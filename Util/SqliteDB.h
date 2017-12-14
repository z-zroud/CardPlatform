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

typedef enum _SQLITE_DATATYPE
{
    SQLITE_DATATYPE_INTEGER = SQLITE_INTEGER,
    SQLITE_DATATYPE_FLOAT   = SQLITE_FLOAT,
    SQLITE_DATATYPE_TEXT    = SQLITE_TEXT,
    SQLITE_DATATYPE_BLOB    = SQLITE_BLOB,
    SQLITE_DATATYPE_NULL    = SQLITE_NULL,
}SQLITE_DATATYPE;

class SQLiteDataReader
{
public:
    SQLiteDataReader(sqlite3_stmt *pStmt);
    ~SQLiteDataReader();
public:
    BOOL    Read();
    void    Close();
    int     ColumnCount(void);
    LPCTSTR GetColumnName(int nCol);
    SQLITE_DATATYPE GetDataType(int nCol);
    LPCTSTR GetStringValue(int nCol);
    int     GetIntValue(int nCol);
    long    GetInt64Value(int nCol);
    double  GetFloatValue(int nCol); 
    const BYTE* GetBlobValue(int nCol, int &nLen);
private:
    sqlite3_stmt *m_pStmt;
};

class SQLite;
class SQLiteCommand
{
public:
    SQLiteCommand(SQLite* pSqlite);
    SQLiteCommand(SQLite* pSqlite, LPCTSTR lpSql);
    ~SQLiteCommand();
public:
    // 设置命令  
    BOOL SetCommandText(LPCTSTR lpSql);
    // 绑定参数（index为要绑定参数的序号，从1开始）  
    BOOL BindParam(int index, LPCTSTR szValue);
    BOOL BindParam(int index, const int nValue);
    BOOL BindParam(int index, const double dValue);
    BOOL BindParam(int index, const unsigned char* blobValue, int nLen);
    // 执行命令  
    BOOL Excute();
    // 清除命令（命令不再使用时需调用该接口清除）  
    void Clear();
private:
    SQLite *m_pSqlite;
    sqlite3_stmt *m_pStmt;
};

class SQLite
{
public:
    friend class SQLiteCommand;
public:
    SQLite(void);
    ~SQLite(void);
public:
    // 打开数据库  
    BOOL Open(LPCTSTR lpDbFlie);
    // 关闭数据库  
    void Close();

    // 执行非查询操作（更新或删除）  
    BOOL ExcuteNonQuery(LPCTSTR lpSql);
    BOOL ExcuteNonQuery(SQLiteCommand* pCmd);

    // 查询  
    SQLiteDataReader ExcuteQuery(LPCTSTR lpSql);
    // 查询（回调方式）  
    BOOL ExcuteQuery(LPCTSTR lpSql, sqlite3_callback pCallBack);
 
    BOOL BeginTransaction(); 
    BOOL CommitTransaction();
    BOOL RollbackTransaction();

    // 获取上一条错误信息  
    LPCTSTR GetLastErrorMsg();

private:
    sqlite3 *m_db;
};


/***********************************************************
* 功能：用于操作KMC表
************************************************************/
class DB_KMC
{
public:
    DB_KMC();
    ~DB_KMC();
    bool GetKmc(vector<string> &vecResult);
    bool AddKmc(string kmc);
    void DeleteKmc(string kmc);

private:
    SQLite    m_db;
};

/***********************************************************
* 功能：用于操作AID表
************************************************************/
class DB_AID
{
public:
    DB_AID();
    ~DB_AID();
    bool GetAid(vector<string> &vecResult);
    bool AddAid(string aid);
    void DeleteAid(string aid);

private:
    SQLite    m_db;
};