#include "StdAfx.h"
#include "EC.h"


EC::EC(IPCSC* pReader) : PBOC(COMM_TOUCH, pReader)
{

}

void EC::ReadECRelativeTags()
{
    vector<string> vecTags = { Tag9F79, Tag9F6D, Tag9F51,Tag9F52, Tag9F53, Tag9F54,Tag9F55,Tag9F56,
    Tag9F57,Tag9F58,Tag9F59,Tag9F5C,Tag9F5D,Tag9F72,Tag9F73,Tag9F75,Tag9F76,Tag9F4F,TagDF4F,Tag9F17,
    Tag9F79,Tag9F77,Tag9F78,TagDF62,TagDF63,TagDF77,TagDF78,TagDF79,TagDF71,TagDF76,TagDF72,Tag9F6D,
    Tag9F36,Tag9F13,Tag9F68,Tag9F6B,TagDF20,TagDF21,TagDF23,TagDF25,TagDF26,TagDF27,TagDF50 };

    for (auto tag : vecTags)
    {
        string value = ReadTagValueFromCard(tag);
        m_tags.insert(pair<string,string>(tag, value));
    }
}

void EC::ReadECRelativeTagsAfterGAC()
{
    vector<string> vecTags = { Tag9F5D, Tag9F77, Tag9F78,Tag9F79};

    for (auto tag : vecTags)
    {
        string value = ReadTagValueFromCard(tag);
        m_tags.insert(pair<string, string>(tag, value));
    }
}


void EC::DoTrans()
{
    SelectApplication();        //��һ���� Ӧ��ѡ��
    InitilizeApplication();     //�ڶ����� Ӧ�ó�ʼ��
    ReadApplicationData();      //�������� ��Ӧ������

    //��Ҫ�ж�EC��������Ȩ���Ƿ����
    if (GetTagValue(Tag9F74).empty())
    {//��������ڣ�ִ��PBOC����

    }
    else {
        //�ڽ�����һ������ʱ����Ҫ��ȡ��Ƭ����EC��ص�TAGֵ
        ReadECRelativeTags();
        OfflineDataAuth();          //���Ĳ��� �ѻ�������֤
        HandleLimitation();         //���岽�� ��������
        CardHolderValidation();     //�������� �ֿ�����֤
        TerminalRiskManagement();   //���߲��� �ն˷��չ���  
        TerminalActionAnalized();   //�ڰ˲��� �ն���Ϊ����
        CardActionAnalized();       //�ھŲ��� ��Ƭ��Ϊ����
        //OnlineBussiness();          //��ʮ���� ��������֤, �����ֽ���Ҫ��������֤
        EndTransaction();           //��ʮһ���� ���׽�������
    }
}