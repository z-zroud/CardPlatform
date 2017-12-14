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
    // ��������  
    BOOL SetCommandText(LPCTSTR lpSql);
    // �󶨲�����indexΪҪ�󶨲�������ţ���1��ʼ��  
    BOOL BindParam(int index, LPCTSTR szValue);
    BOOL BindParam(int index, const int nValue);
    BOOL BindParam(int index, const double dValue);
    BOOL BindParam(int index, const unsigned char* blobValue, int nLen);
    // ִ������  
    BOOL Excute();
    // �����������ʹ��ʱ����øýӿ������  
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
    // �����ݿ�  
    BOOL Open(LPCTSTR lpDbFlie);
    // �ر����ݿ�  
    void Close();

    // ִ�зǲ�ѯ���������»�ɾ����  
    BOOL ExcuteNonQuery(LPCTSTR lpSql);
    BOOL ExcuteNonQuery(SQLiteCommand* pCmd);

    // ��ѯ  
    SQLiteDataReader ExcuteQuery(LPCTSTR lpSql);
    // ��ѯ���ص���ʽ��  
    BOOL ExcuteQuery(LPCTSTR lpSql, sqlite3_callback pCallBack);
 
    BOOL BeginTransaction(); 
    BOOL CommitTransaction();
    BOOL RollbackTransaction();

    // ��ȡ��һ��������Ϣ  
    LPCTSTR GetLastErrorMsg();

private:
    sqlite3 *m_db;
};


/***********************************************************
* ���ܣ����ڲ���KMC��
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
* ���ܣ����ڲ���AID��
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