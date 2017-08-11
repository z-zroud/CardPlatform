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
struct ININode 
{ 
    ININode(string root, string key, string value) 
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
		m_Dict.insert(pair<string, string>(key, value));
    } 

    map<string, string> m_Dict; 
}; 

  

class INIParser 
{ 
public:
	INIParser();
	INIParser(string path);
public: 
    int Read(string path); 
	int Read();
    string GetValue(string root, string key); 
    vector<ININode>::size_type GetSize(){return m_Section.size();} 
    vector<ININode>::size_type SetValue(string root, string key, string value); 
    int Save(string path); 
    inline void Clear(){m_Section.clear();} 

private:
    bool IsExisted(string root, string key);

private: 
    //map<string, SubNode> m_Section; 
    vector < pair<string, SubNode>> m_Section;
	string m_filePath;
}; 

#endif // INI_PARSER_H 

