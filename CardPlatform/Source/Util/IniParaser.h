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
  
//ÿ��ini�ڵ�
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

  
// key=value �ڵ�����
struct SubNode 
{ 
    void InsertElement(string key, string value)
    {
        //m_Dict.insert(pair<string, string>(key, value));
        m_Dict.push_back(pair<string, string>(key, value));
    }

    vector<pair<string, string>> m_Dict;
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
    vector<pair<string, SubNode>> GetAllNodes() const { return m_Section; }
    int Save(string path); 
    inline void Clear(){m_Section.clear();} 

private:
private:
    bool IsExisted(string root, string key);
    vector<pair<string, string>>::iterator IsExisted(vector<pair<string, string>> &vec, string key);

private: 
    //map<string, SubNode> m_Section; 
    vector < pair<string, SubNode>> m_Section;
	string m_filePath;
}; 

#endif // INI_PARSER_H 

