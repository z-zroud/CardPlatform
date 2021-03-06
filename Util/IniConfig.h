#ifndef INI_PARSER_H 

#define INI_PARSER_H 

#include <iostream> 
#include <fstream> 
#include <sstream> 
#include <vector> 
#include <cstdlib> 
#include <map> 

using namespace std; 

#define INI_OK  1
#define INI_ERROR   0
  
//每个ini节点
struct IniNode 
{ 
    IniNode(string root, string key, string value) 
    { 
		m_Root	= root;
		m_Key	= key;
		m_Value	= value;
    } 

    string m_Root; 
    string m_Key; 
    string m_Value; 
}; 

  
// key=value 节点容器
struct SubNode 
{ 
    void InsertElement(string key, string value)
    {
        m_collection.push_back(pair<string, string>(key, value));
    }

    vector<pair<string, string>> m_collection;
}; 

  

class IniConfig 
{ 
public:
	IniConfig();
	IniConfig(string path);
public: 
    bool Read(string path); 
    bool IsOpened() { return m_bOpened; }
	int Read();
    string GetValue(string root, string key); 
    void GetValue(string root, vector<pair<string,string>>& nodes);
    vector<IniNode>::size_type GetSize(){return m_vecSection.size();}
    vector<IniNode>::size_type SetValue(string root, string key, string value); 
    vector<pair<string, SubNode>> GetAllNodes() const { return m_vecSection; }
    int Save(string path); 
	void Save();
    inline void Clear(){ m_vecSection.clear();}

private:
    bool IsExisted(string root, string key);
    vector<pair<string, string>>::iterator IsExisted(vector<pair<string, string>> &vec, string key);

private: 
    vector < pair<string, SubNode>> m_vecSection;
	string m_filePath;
    bool m_bOpened;
}; 

#endif // INI_PARSER_H 

