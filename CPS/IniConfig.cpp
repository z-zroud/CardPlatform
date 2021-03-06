#include "stdafx.h"
#include "IniConfig.h"

  
//删除字符串两端空白 
string &TrimString(string &str) 
{ 
    string::size_type pos = 0; 
    while(str.npos != (pos = str.find(" "))) 
        str = str.replace(pos, pos+1, ""); 
    return str; 
} 

IniConfig::IniConfig()
{

}

IniConfig::IniConfig(string filePath)
{
	m_filePath = filePath;
}

int IniConfig::Read()
{
	if (!m_filePath.empty())
	{
		return Read(m_filePath);
	}

	return -1;
}

//读ini文件
bool IniConfig::Read(string path) 
{ 
    ifstream iniFile(path.c_str()); 
    if(!iniFile) 
		return false;

	m_filePath = path;
    string strLine; 
    string strRoot; 
    vector<IniNode> vec_ini; 
    while(getline(iniFile, strLine))	//逐行读取
    { 
        string::size_type leftPos = 0; 
        string::size_type rightPos = 0; 
        string::size_type equalDivPos = 0; 
        string str_key = ""; 
        string str_value = ""; 

		//是否为root
        if((strLine.npos != (leftPos = strLine.find("["))) && 
			(strLine.npos != (rightPos = strLine.find("]")))&&
			leftPos == 0)
        { 
            strRoot = strLine.substr(leftPos + 1, rightPos - 1); 
        } 
        else if(strLine.npos != (equalDivPos = strLine.find("=")))	// key=value
        { 
           str_key = TrimString(strLine.substr(0, equalDivPos));
           str_value = strLine.substr(equalDivPos + 1, strLine.size() - 1); 
        } 

        if((!strRoot.empty()) && 
			(!str_key.empty()) && 
			(!str_value.empty())) 
        { 
			IniNode iniNode(strRoot, str_key, str_value); 
			vec_ini.push_back(iniNode);
        } 
    } 

    iniFile.close(); 
    iniFile.clear(); 

	//保存ini配置信息到m_Section容器中
	if (vec_ini.size() == 0)
	{
		return false;	//无配置信息
	}
	SubNode sn;
	strRoot = vec_ini.begin()->m_Root;
	for (auto iter : vec_ini)
	{
		if (strRoot == iter.m_Root)
		{
			sn.InsertElement(iter.m_Key, iter.m_Value);
		}
		else {
			m_vecSection.push_back(pair<string, SubNode>(strRoot, sn));
			strRoot = iter.m_Root;
			sn.m_collection.clear();
			sn.InsertElement(iter.m_Key, iter.m_Value);
		}
	}
	//插入最后一个节点
	m_vecSection.push_back(pair<string, SubNode>(strRoot, sn));
    m_bOpened = true;
    return true; 
} 
 

//通过root和key获取value 
string IniConfig::GetValue(string root, string key) 
{ 
    for (auto v : m_vecSection)
    {
        if (v.first == root)
        {
            auto subIter = IsExisted(v.second.m_collection, key);
            if (subIter == v.second.m_collection.end())
            {
                return "";
            }
            if (!(subIter->second).empty())
            {
                return subIter->second;
            }
        }
    }

    return "";
} 

void IniConfig::GetValue(string root, vector<pair<string,string>>& nodes)
{
    for (auto v : m_vecSection)
    {
        if (v.first == root)
        {
            for (auto item : v.second.m_collection)
            {
                nodes.push_back(item);
            }
        }
    }
}
 
void IniConfig::Save()
{
	Save(m_filePath);
}

//写ini文件
int IniConfig::Save(string path) 
{ 
    ofstream iniFile(path.c_str()); 
    if(!iniFile)
        return -1; 
    string flag = "";
    for(auto iter = m_vecSection.begin(); iter != m_vecSection.end(); ++iter)
    { 
        if (iter->first != flag)
        {
            iniFile << "[" << iter->first << "]" << endl;
            flag = iter->first;
        }
		
       for(auto subIter = iter->second.m_collection.begin(); subIter != iter->second.m_collection.end(); ++subIter)
       { 
		   iniFile << subIter->first << "=" << subIter->second << endl;
       } 
    } 

	iniFile.close();
	iniFile.clear();

    return 1; 
} 

vector<pair<string, string>>::iterator IniConfig::IsExisted(vector<pair<string, string>> &vec, string key)
{
    for (auto iter = vec.begin(); iter != vec.end(); iter++)
    {
        if (iter->first == key)
            return iter;
    }

    return vec.end();
}

bool IniConfig::IsExisted(string root, string key)
{
    for (auto v : m_vecSection)
    {
        if (v.first == root)
        {
            auto m = IsExisted(v.second.m_collection, key);
            if (m != v.second.m_collection.end())
            {
                return true;
            }
        }
    }

    return false;
}

//设置值
vector<IniNode>::size_type IniConfig::SetValue(string root, string key, string value) 
{ 
    if (IsExisted(root, key))
    {
        for (auto v : m_vecSection)
        {
            if (v.first == root)
            {
                auto m = IsExisted(v.second.m_collection, key);
                if (m != v.second.m_collection.end())
                {
                    m->second = value;
                }
            }
        }
    }
    else {
        SubNode sn;
        sn.InsertElement(key, value);
		m_vecSection.push_back(pair<string, SubNode>(root, sn));
    }

    return 0;
} 
