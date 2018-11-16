#include "stdafx.h"
#include "ParseTLV.h"
#include "Tool.h"
#include <string>

bool ParseBcdTLV(char* buffer, PBCD_TLV pTlvs, unsigned int& count)
{
    unsigned int	currentIndex    = 0;                //���ڱ��buffer
    unsigned long	dataSize        = 0;                //���ݳ���
    int				currentTLVIndex = 0;                //��ǰTLV�ṹ���
    int				currentStatus   = 'T';              //״̬�ַ�
    unsigned int bufferLen          = strlen(buffer);

    while (currentIndex < bufferLen)
    {
        switch (currentStatus)
        {
        case 'T':
            dataSize = 0; //����			
            if (Tool::ctoi(buffer[currentIndex]) & 0x02)	//�ж�TLV�Ƿ�Ϊ��һ�ṹ,�ֽڵĵ�6λ�Ƿ�Ϊ1
            {//���Ͻṹ ˵����ģ��				
                pTlvs[currentTLVIndex].isTemplate = true;
                unsigned int tempIndex = currentIndex;
                if ((Tool::ctoi(buffer[tempIndex]) & 0x01) && (Tool::ctoi(buffer[++tempIndex]) == 0x0F))	//�ж�tag�Ƿ�Ϊ���ֽ�
                {//tagΪ���ֽ�					
                    int endTagIndex = tempIndex + 1;
                    while (Tool::ctoi(buffer[endTagIndex]) & 0x08)
                    {
                        endTagIndex += 2;
                    }
                    int tagSize = endTagIndex - currentIndex + 2;

                    pTlvs[currentTLVIndex].tag = (char *)malloc(tagSize + 1);
                    memset(pTlvs[currentTLVIndex].tag, 0, tagSize + 1);
                    memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, tagSize);
                    pTlvs[currentTLVIndex].tagSize = tagSize;
                    currentIndex += tagSize;
                }
                else
                { //tagΪ���ֽ�
                    pTlvs[currentTLVIndex].tag = (char *)malloc(3);
                    memset(pTlvs[currentTLVIndex].tag, 0, 3);
                    memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, 2);
                    pTlvs[currentTLVIndex].tagSize = 2;
                    currentIndex += 2;
                }

                //������TLV�е�Tag
                int subTlvLength = 0;			//��TLV����
                char * temp;			//��TLV������������
                //���ж�length��ĳ���,length���ֽ�������λΪ1�������ֽڴ����ȣ�Ϊ0��1--7λ�������ݳ���
                unsigned int nStartIndex = currentIndex;
                int lenOffset = 0;
                if (Tool::ctoi(buffer[nStartIndex]) & 0x08)
                {
                    //���λ1
                    int height = (Tool::ctoi(buffer[nStartIndex]) & 0x07) * 8;
                    int low = Tool::ctoi(buffer[++nStartIndex]) & 0x0f;
                    unsigned int lengthSize = 2 * (height + low);

                    nStartIndex += 1; //����һ���ֽڿ�ʼ��Length��
                    pTlvs[currentTLVIndex].length = (char *)malloc(lengthSize + 1);
                    memset(pTlvs[currentTLVIndex].length, 0, lengthSize + 1);
                    memcpy(pTlvs[currentTLVIndex].length, buffer + nStartIndex, lengthSize);
                    pTlvs[currentTLVIndex].lenSize = lengthSize;

                    subTlvLength = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);


                    //����һ��subTlvlength��С���ڴ��Ÿ�TLV������
                    temp = (char *)malloc(subTlvLength + 1);
                    memset(temp, 0, subTlvLength + 1);
                    memcpy(temp, buffer + nStartIndex + lengthSize, subTlvLength);
                    lenOffset = nStartIndex - currentIndex;
                }
                else
                {
                    //���λΪ0
                    char subTlvLen[3] = { 0 };
                    memcpy(subTlvLen, &buffer[currentIndex], 2);
                    subTlvLength = 2 * std::stoi(subTlvLen, 0, 16);
                    temp = (char *)malloc(subTlvLength + 1);
                    memset(temp, 0, subTlvLength + 1);
                    memcpy(temp, buffer + currentIndex + 2, subTlvLength);
                    pTlvs[currentTLVIndex].lenSize = 2;

                }
                //if (subTlvLength + currentIndex + lenOffset + pTlvs[currentTLVIndex].lenSize != bufferLen) {
                //    return false;
                //}
                temp[subTlvLength] = 0;

                unsigned int oSize;//����ж��ٸ�ͬ�ȼ�����TLV������ʱҲӦ���õ�

                //�������TLVͬ�ȼ���TLV�ж��ٸ�������32TLV��С���ڴ�϶�����
                pTlvs[currentTLVIndex].subTLVEntity = new BCD_TLV[sizeof(PBCD_TLV[32])];
                memset(pTlvs[currentTLVIndex].subTLVEntity, 0, sizeof(PBCD_TLV[32]));
                if (!ParseBcdTLV((char*)temp, pTlvs[currentTLVIndex].subTLVEntity, oSize)) {
                    return false;
                }

                pTlvs[currentTLVIndex].subTLVnum = oSize; //������TLV������
            }
            else
            {
                //��һ�ṹ
                pTlvs[currentTLVIndex].isTemplate = false;
                pTlvs[currentTLVIndex].subTLVEntity = NULL;
                pTlvs[currentTLVIndex].subTLVnum = 0;

                unsigned int tempIndex = currentIndex;
                if ((Tool::ctoi(buffer[tempIndex]) & 0x01) && (Tool::ctoi(buffer[++tempIndex]) == 0x0F))
                {
                    //���ֽ�
                    int endTagIndex = tempIndex + 1;
                    while (Tool::ctoi(buffer[endTagIndex]) & 0x08)
                    {
                        endTagIndex += 2;
                    }
                    int tagSize = endTagIndex - currentIndex + 2;

                    pTlvs[currentTLVIndex].tag = (char *)malloc(tagSize + 1);
                    memset(pTlvs[currentTLVIndex].tag, 0, tagSize + 1);
                    memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, tagSize);
                    

                    pTlvs[currentTLVIndex].tagSize = tagSize;

                    currentIndex += tagSize;
                }
                else
                {
                    //���ֽ�
                    pTlvs[currentTLVIndex].tag = (char *)malloc(3);
                    memset(pTlvs[currentTLVIndex].tag, 0, 3);
                    memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, 2);
                    

                    pTlvs[currentTLVIndex].tagSize = 2;

                    currentIndex += 2;
                }
            }
            currentStatus = 'L';
            break;
        case 'L':
            //�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
            if (Tool::ctoi(buffer[currentIndex]) & 0x08)
            {
                //���λ1
                int height = (Tool::ctoi(buffer[currentIndex]) & 0x07) * 8;
                int low = Tool::ctoi(buffer[++currentIndex]) & 0x0f;
                unsigned int lengthSize = 2 * (height + low);

                currentIndex += 1; //����һ���ֽڿ�ʼ��Length��
                pTlvs[currentTLVIndex].length = (char *)malloc(lengthSize + 1);
                memset(pTlvs[currentTLVIndex].length, 0, lengthSize + 1);
                memcpy(pTlvs[currentTLVIndex].length, buffer + currentIndex, lengthSize);
                
                pTlvs[currentTLVIndex].lenSize = lengthSize;
                if (strlen(pTlvs[currentTLVIndex].length) > 2)  //���ݳ���һ�㲻���ܴ���FF
                    return false;
                dataSize = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

                currentIndex += lengthSize;

            }
            else
            {
                //���λ0
                pTlvs[currentTLVIndex].length = (char *)malloc(3);
                memset(pTlvs[currentTLVIndex].length, 0, 3);
                memcpy(pTlvs[currentTLVIndex].length, buffer + currentIndex, 2);
                
                pTlvs[currentTLVIndex].lenSize = 2;

                dataSize = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

                currentIndex += 2;
            }
            if (dataSize + currentIndex > bufferLen) {  //��ȷ��TLV��ʽ����Ӧ�ó���dataSize ���� bufferLen
                return false;
            }
            //if (pTlvs[currentTLVIndex].isTemplate) {    //������ͬ��ģ�壬�����ģ�壬������ģ�����������
            //    if (currentIndex + dataSize != bufferLen) {
            //        return false;
            //    }
            //}
            currentStatus = 'V';
            break;
        case 'V':
            pTlvs[currentTLVIndex].value = (char *)malloc(dataSize + 1);
            memset(pTlvs[currentTLVIndex].value, 0, dataSize + 1);
            memcpy(pTlvs[currentTLVIndex].value, buffer + currentIndex, dataSize);
            

            currentIndex += dataSize;

            //������һ��TLV����ѭ��
            currentTLVIndex += 1;

            currentStatus = 'T';
            break;
        }
    }
    count = currentTLVIndex;
    return true;
}

bool IsBcdTlvStruct(char* buffer, unsigned int bufferLength)
{
    unsigned int currentIndex = 0;							//���ڱ��buffer
    if (bufferLength < 2) {
        return false;
    }
    char appTag[3] = { 0 };
    strncpy(appTag, buffer, 2);
    if (stoi(appTag, 0, 16) < 0x4F) {   //��Ӧ�ó���Universal���͵�tag,����Ӧ��ֻ����Ӧ�����͵�tag
        return false;
    }
    while (currentIndex < bufferLength)//�ж���Tag�Ƿ�Ϊ���ֽ�,�ֽڵ�1--5λ�Ƿ�Ϊ1���ǵĻ��к����ֽ�
    {
        unsigned int tempIndex = currentIndex;
        if ((Tool::ctoi(buffer[tempIndex]) & 0x01) && (Tool::ctoi(buffer[++tempIndex]) == 0x0F))  //tagΪ���ֽ�
        {
            int endTagIndex = tempIndex + 1;
            while (Tool::ctoi(buffer[endTagIndex]) & 0x08)
            {
                endTagIndex += 2;
            }
            int tagSize = endTagIndex - currentIndex + 2;
            currentIndex += tagSize;
        }
        else       //tagΪ���ֽ�
        {
            currentIndex += 2;
        }

        //����Length�򳤶�
        int dataLen = 0;
        unsigned int lenSize = 0;
        if (Tool::ctoi(buffer[currentIndex]) & 0x08)
        {
            //���λΪ1
            int height = (Tool::ctoi(buffer[currentIndex]) & 0x07) * 8;
            int low = Tool::ctoi(buffer[++currentIndex]) & 0x0f;
            lenSize = 2 * (height + low);
            
            currentIndex += 1;
            //currentIndex += lenSize;
        }
        else  //���λΪ0
        {
            lenSize = 2;
            //currentIndex += lenSize;           
        }
        if (lenSize > 4 || lenSize == 0) {
            return false;   //��ʱ��֧��65535���ȵ�TLV
        }
        if (lenSize + currentIndex > bufferLength)
        {
            return false;
        }
        char* szDataLen = new char[lenSize + 1];
        memset(szDataLen, 0, lenSize + 1);
        memcpy(szDataLen, buffer + currentIndex, lenSize);
        dataLen = 2 * stoi(szDataLen, 0, 16);
        if (dataLen + currentIndex + lenSize > bufferLength) {
            return false;
        }
        currentIndex += lenSize + dataLen;
    }

    return currentIndex == bufferLength;
}


/**********************************************************************
* ����TLV���ݽṹ
***********************************************************************/
void ParseAsciiTLV(unsigned char *buffer,			//TLV�ַ���	
	unsigned int bufferLength,		//TLV�ַ�������
	PASCII_TLV PTlvEntity,					//TLVָ��
	unsigned int& entitySize			//TLV�ṹ����������ʱ�õ�
	)
{
	unsigned int currentIndex = 0;							//���ڱ��buffer
	int currentTLVIndex = 0;						//��ǰTLV�ṹ���
	int currentStatus = 'T';						//״̬�ַ�

	unsigned long valueSize = 0;					//���ݳ���

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0; //����
						   //�ж�TLV�Ƿ�Ϊ��һ�ṹ,�ֽڵĵ�6λ�Ƿ�Ϊ1
			if ((buffer[currentIndex] & 0x20) != 0x20)
			{
				//��һ�ṹ
				PTlvEntity[currentTLVIndex].subTLVEntity = NULL;
				PTlvEntity[currentTLVIndex].subTLVnum = 0;		//��TLV������Ϊ��
																//�ж���Tag�Ƿ�Ϊ���ֽ�,�ֽڵ�1--5λ�Ƿ�Ϊ1���ǵĻ��к����ֽ�
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					//TagΪ���ֽ�
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //���һ���ֽڵ����λΪ0
					int tagSize = endTagIndex - currentIndex + 1; //�����ǩ��ռ���ֽ�

					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].tag[tagSize] = 0;//�ַ���ĩβ��0

					PTlvEntity[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//Tagռ��1���ֽ�
					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].tag[1] = 0;

					PTlvEntity[currentTLVIndex].tagSize = 1;

					currentIndex += 1;
				}
			}
			else
			{
				//���Ͻṹ
				//�ж��Ƿ�Ϊ���ֽ�
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80);
					int tagSize = endTagIndex - currentIndex + 1;

					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].tag[tagSize] = 0;

					PTlvEntity[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].tag[1] = 0;

					PTlvEntity[currentTLVIndex].tagSize = 1;

					currentIndex += 1;
				}

				//������TLV�е�Tag
				int subTlvLength = 0;			//��TLV����
				unsigned char * temp;			//��TLV������������
												//���ж�length��ĳ���,length���ֽ�������λΪ1�������ֽڴ����ȣ�Ϊ0��1--7λ�������ݳ���
				if ((buffer[currentIndex] & 0x80) == 0x80)
				{
					//���λΪ1
					unsigned int lengthSize = buffer[currentIndex] & 0x7f;
					for (unsigned int index = 0; index < lengthSize; index++)
					{
						//�����ʾ����
						subTlvLength += buffer[currentIndex + 1 + index] << ((lengthSize - 1 - index) * 8);

						/* �����С�˵Ļ�
						subTlvLength += buffer[currentIndex + 1 + index] << (index * 8);
						*/
					}


					//����һ��subTlvlength��С���ڴ��Ÿ�TLV������
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + currentIndex + 1 + lengthSize, subTlvLength);

				}
				else
				{
					//���λΪ0
					subTlvLength = buffer[currentIndex];
					temp = (unsigned char *)malloc(subTlvLength);

					memcpy(temp, buffer + currentIndex + 1, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//����ж��ٸ�ͬ�ȼ�����TLV������ʱҲӦ���õ�
								   //�������TLVͬ�ȼ���TLV�ж��ٸ�������100TLV��С���ڴ�϶�����
				PTlvEntity[currentTLVIndex].subTLVEntity = (PASCII_TLV)malloc(sizeof(PASCII_TLV[100]));
                ParseAsciiTLV(temp, subTlvLength, PTlvEntity[currentTLVIndex].subTLVEntity, oSize);

				PTlvEntity[currentTLVIndex].subTLVnum = oSize; //������TLV������

			}
			currentStatus = 'L';
			break;
		case 'L':
			//�жϳ����ֽڵ����λ�Ƿ�Ϊ1�����Ϊ1������ֽ�Ϊ������չ�ֽڣ�����һ���ֽڿ�ʼ��������
			if ((buffer[currentIndex] & 0x80) == 0x80)
			{
				//���λ1
				unsigned int lengthSize = buffer[currentIndex] & 0x7f;
				currentIndex += 1; //����һ���ֽڿ�ʼ��Length��
				for (unsigned int index = 0; index < lengthSize; index++)
				{
					valueSize += buffer[currentIndex + index] << ((lengthSize - 1 - index) * 8); //����Length��ĳ���
				}
				PTlvEntity[currentTLVIndex].length = (unsigned char *)malloc(lengthSize);
				memcpy(PTlvEntity[currentTLVIndex].length, buffer + currentIndex, lengthSize);
				PTlvEntity[currentTLVIndex].length[lengthSize] = 0;
				PTlvEntity[currentTLVIndex].lengthSize = lengthSize;

				currentIndex += lengthSize;
			}
			else
			{
				//���λ0
				PTlvEntity[currentTLVIndex].length = (unsigned char *)malloc(1);
				memcpy(PTlvEntity[currentTLVIndex].length, buffer + currentIndex, 1);
				PTlvEntity[currentTLVIndex].length[1] = 0;
				PTlvEntity[currentTLVIndex].lengthSize = 1;

				valueSize = PTlvEntity[currentTLVIndex].length[0];

				currentIndex += 1;
			}
			currentStatus = 'V';
			break;
		case 'V':
			PTlvEntity[currentTLVIndex].value = (unsigned char *)malloc(valueSize);
			memset(PTlvEntity[currentTLVIndex].value, 0, valueSize);
			memcpy(PTlvEntity[currentTLVIndex].value, buffer + currentIndex, valueSize);
			PTlvEntity[currentTLVIndex].value[valueSize] = 0;

			currentIndex += valueSize;

			//������һ��TLV����ѭ��
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}

/****************************************************************
* �ж�ĳ�������Ƿ�ΪTLV�ṹ
*****************************************************************/
bool IsAsciiTlvStruct(unsigned char* buffer, unsigned int bufferLength)
{
    unsigned int currentIndex = 0;							//���ڱ��buffer
                                                            //�ж���Tag�Ƿ�Ϊ���ֽ�,�ֽڵ�1--5λ�Ƿ�Ϊ1���ǵĻ��к����ֽ�
    while (currentIndex < bufferLength)
    {
        if ((buffer[currentIndex] & 0x1f) == 0x1f)  //tagΪ���ֽ�
        {
            int endTagIndex = currentIndex;
            while ((buffer[++endTagIndex] & 0x80) == 0x80); //���һ���ֽڵ����λΪ0
            int tagSize = endTagIndex - currentIndex + 1; //�����ǩ��ռ���ֽ�
            currentIndex += tagSize;
        }
        else       //tagΪ���ֽ�
        {
            currentIndex += 1;
        }

        int dataLen = 0;
        unsigned int lenSize = 0;
        //����Length�򳤶�
        if ((buffer[currentIndex] & 0x80) == 0x80)
        {
            //���λΪ1
            lenSize = buffer[currentIndex] & 0x7f;
            for (unsigned int index = 0; index < lenSize; index++)
            {
                dataLen += buffer[currentIndex + 1 + index] << ((lenSize - 1 - index) * 8);
            }
            lenSize += 1;
        }
        else  //���λΪ0
        {
            dataLen = buffer[currentIndex];
            lenSize = 1;
        }
        currentIndex += lenSize + dataLen;
    }

    return currentIndex == bufferLength;
}