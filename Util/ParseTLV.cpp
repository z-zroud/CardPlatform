#include "stdafx.h"
#include "ParseTLV.h"
#include "Tool.h"
#include <string>

bool ParseBcdTLV(char* buffer, PBCD_TLV pTlvs, unsigned int& count)
{
    unsigned int	currentIndex    = 0;                //用于标记buffer
    unsigned long	dataSize        = 0;                //数据长度
    int				currentTLVIndex = 0;                //当前TLV结构标记
    int				currentStatus   = 'T';              //状态字符
    unsigned int bufferLen          = strlen(buffer);

    while (currentIndex < bufferLen)
    {
        switch (currentStatus)
        {
        case 'T':
            dataSize = 0; //清零			
            if (Tool::ctoi(buffer[currentIndex]) & 0x02)	//判断TLV是否为单一结构,字节的第6位是否为1
            {//复合结构 说明是模板				
                pTlvs[currentTLVIndex].isTemplate = true;
                unsigned int tempIndex = currentIndex;
                if ((Tool::ctoi(buffer[tempIndex]) & 0x01) && (Tool::ctoi(buffer[++tempIndex]) == 0x0F))	//判断tag是否为多字节
                {//tag为多字节					
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
                { //tag为单字节
                    pTlvs[currentTLVIndex].tag = (char *)malloc(3);
                    memset(pTlvs[currentTLVIndex].tag, 0, 3);
                    memcpy(pTlvs[currentTLVIndex].tag, buffer + currentIndex, 2);
                    pTlvs[currentTLVIndex].tagSize = 2;
                    currentIndex += 2;
                }

                //分析子TLV中的Tag
                int subTlvLength = 0;			//子TLV长度
                char * temp;			//子TLV所包含的数据
                //先判断length域的长度,length域字节如果最高位为1，后续字节代表长度，为0，1--7位代表数据长度
                unsigned int nStartIndex = currentIndex;
                int lenOffset = 0;
                if (Tool::ctoi(buffer[nStartIndex]) & 0x08)
                {
                    //最高位1
                    int height = (Tool::ctoi(buffer[nStartIndex]) & 0x07) * 8;
                    int low = Tool::ctoi(buffer[++nStartIndex]) & 0x0f;
                    unsigned int lengthSize = 2 * (height + low);

                    nStartIndex += 1; //从下一个字节开始算Length域
                    pTlvs[currentTLVIndex].length = (char *)malloc(lengthSize + 1);
                    memset(pTlvs[currentTLVIndex].length, 0, lengthSize + 1);
                    memcpy(pTlvs[currentTLVIndex].length, buffer + nStartIndex, lengthSize);
                    pTlvs[currentTLVIndex].lenSize = lengthSize;

                    subTlvLength = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);


                    //申请一段subTlvlength大小的内存存放该TLV的内容
                    temp = (char *)malloc(subTlvLength + 1);
                    memset(temp, 0, subTlvLength + 1);
                    memcpy(temp, buffer + nStartIndex + lengthSize, subTlvLength);
                    lenOffset = nStartIndex - currentIndex;
                }
                else
                {
                    //最高位为0
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

                unsigned int oSize;//输出有多少个同等级的子TLV，解析时也应该用到

                //不清楚子TLV同等级的TLV有多少个，申请32TLV大小的内存肯定够用
                pTlvs[currentTLVIndex].subTLVEntity = new BCD_TLV[sizeof(PBCD_TLV[32])];
                memset(pTlvs[currentTLVIndex].subTLVEntity, 0, sizeof(PBCD_TLV[32]));
                if (!ParseBcdTLV((char*)temp, pTlvs[currentTLVIndex].subTLVEntity, oSize)) {
                    return false;
                }

                pTlvs[currentTLVIndex].subTLVnum = oSize; //填入子TLV的数量
            }
            else
            {
                //单一结构
                pTlvs[currentTLVIndex].isTemplate = false;
                pTlvs[currentTLVIndex].subTLVEntity = NULL;
                pTlvs[currentTLVIndex].subTLVnum = 0;

                unsigned int tempIndex = currentIndex;
                if ((Tool::ctoi(buffer[tempIndex]) & 0x01) && (Tool::ctoi(buffer[++tempIndex]) == 0x0F))
                {
                    //多字节
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
                    //单字节
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
            //判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
            if (Tool::ctoi(buffer[currentIndex]) & 0x08)
            {
                //最高位1
                int height = (Tool::ctoi(buffer[currentIndex]) & 0x07) * 8;
                int low = Tool::ctoi(buffer[++currentIndex]) & 0x0f;
                unsigned int lengthSize = 2 * (height + low);

                currentIndex += 1; //从下一个字节开始算Length域
                pTlvs[currentTLVIndex].length = (char *)malloc(lengthSize + 1);
                memset(pTlvs[currentTLVIndex].length, 0, lengthSize + 1);
                memcpy(pTlvs[currentTLVIndex].length, buffer + currentIndex, lengthSize);
                
                pTlvs[currentTLVIndex].lenSize = lengthSize;
                dataSize = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

                currentIndex += lengthSize;

            }
            else
            {
                //最高位0
                pTlvs[currentTLVIndex].length = (char *)malloc(3);
                memset(pTlvs[currentTLVIndex].length, 0, 3);
                memcpy(pTlvs[currentTLVIndex].length, buffer + currentIndex, 2);
                
                pTlvs[currentTLVIndex].lenSize = 2;

                dataSize = 2 * std::stoi((char*)pTlvs[currentTLVIndex].length, 0, 16);

                currentIndex += 2;
            }
            if (dataSize + currentIndex > bufferLen) {  //正确的TLV格式，不应该出现dataSize 大于 bufferLen
                return false;
            }
            //if (pTlvs[currentTLVIndex].isTemplate) {    //不存在同级模板，如果有模板，不存在模板以外的数据
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

            //进入下一个TLV构造循环
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
    unsigned int currentIndex = 0;							//用于标记buffer
    if (bufferLength < 2) {
        return false;
    }
    char appTag[3] = { 0 };
    strncpy(appTag, buffer, 2);
    if (stoi(appTag, 0, 16) < 0x4F) {   //不应该出现Universal类型的tag,这里应该只包含应用类型的tag
        return false;
    }
    while (currentIndex < bufferLength)//判断是Tag是否为多字节,字节的1--5位是否都为1，是的话有后续字节
    {
        unsigned int tempIndex = currentIndex;
        if ((Tool::ctoi(buffer[tempIndex]) & 0x01) && (Tool::ctoi(buffer[++tempIndex]) == 0x0F))  //tag为多字节
        {
            int endTagIndex = tempIndex + 1;
            while (Tool::ctoi(buffer[endTagIndex]) & 0x08)
            {
                endTagIndex += 2;
            }
            int tagSize = endTagIndex - currentIndex + 2;
            currentIndex += tagSize;
        }
        else       //tag为单字节
        {
            currentIndex += 2;
        }

        //计算Length域长度
        int dataLen = 0;
        unsigned int lenSize = 0;
        if (Tool::ctoi(buffer[currentIndex]) & 0x08)
        {
            //最高位为1
            int height = (Tool::ctoi(buffer[currentIndex]) & 0x07) * 8;
            int low = Tool::ctoi(buffer[++currentIndex]) & 0x0f;
            lenSize = 2 * (height + low);
            
            currentIndex += 1;
            //currentIndex += lenSize;
        }
        else  //最高位为0
        {
            lenSize = 2;
            //currentIndex += lenSize;           
        }
        if (lenSize > 4 || lenSize == 0) {
            return false;   //暂时不支持65535长度的TLV
        }
        if (lenSize + currentIndex >= bufferLength)
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
* 解析TLV数据结构
***********************************************************************/
void ParseAsciiTLV(unsigned char *buffer,			//TLV字符串	
	unsigned int bufferLength,		//TLV字符串长度
	PASCII_TLV PTlvEntity,					//TLV指针
	unsigned int& entitySize			//TLV结构数量，解析时用到
	)
{
	unsigned int currentIndex = 0;							//用于标记buffer
	int currentTLVIndex = 0;						//当前TLV结构标记
	int currentStatus = 'T';						//状态字符

	unsigned long valueSize = 0;					//数据长度

	while (currentIndex < bufferLength)
	{
		switch (currentStatus)
		{
		case 'T':
			valueSize = 0; //清零
						   //判断TLV是否为单一结构,字节的第6位是否为1
			if ((buffer[currentIndex] & 0x20) != 0x20)
			{
				//单一结构
				PTlvEntity[currentTLVIndex].subTLVEntity = NULL;
				PTlvEntity[currentTLVIndex].subTLVnum = 0;		//子TLV的数量为零
																//判断是Tag是否为多字节,字节的1--5位是否都为1，是的话有后续字节
				if ((buffer[currentIndex] & 0x1f) == 0x1f)
				{
					//Tag为多字节
					int endTagIndex = currentIndex;
					while ((buffer[++endTagIndex] & 0x80) == 0x80); //最后一个字节的最高位为0
					int tagSize = endTagIndex - currentIndex + 1; //计算标签所占用字节

					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(tagSize);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, tagSize);
					PTlvEntity[currentTLVIndex].tag[tagSize] = 0;//字符串末尾置0

					PTlvEntity[currentTLVIndex].tagSize = tagSize;

					currentIndex += tagSize;
				}
				else
				{
					//Tag占用1个字节
					PTlvEntity[currentTLVIndex].tag = (unsigned char *)malloc(1);
					memcpy(PTlvEntity[currentTLVIndex].tag, buffer + currentIndex, 1);
					PTlvEntity[currentTLVIndex].tag[1] = 0;

					PTlvEntity[currentTLVIndex].tagSize = 1;

					currentIndex += 1;
				}
			}
			else
			{
				//复合结构
				//判断是否为多字节
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

				//分析子TLV中的Tag
				int subTlvLength = 0;			//子TLV长度
				unsigned char * temp;			//子TLV所包含的数据
												//先判断length域的长度,length域字节如果最高位为1，后续字节代表长度，为0，1--7位代表数据长度
				if ((buffer[currentIndex] & 0x80) == 0x80)
				{
					//最高位为1
					unsigned int lengthSize = buffer[currentIndex] & 0x7f;
					for (unsigned int index = 0; index < lengthSize; index++)
					{
						//大端显示数据
						subTlvLength += buffer[currentIndex + 1 + index] << ((lengthSize - 1 - index) * 8);

						/* 如果是小端的话
						subTlvLength += buffer[currentIndex + 1 + index] << (index * 8);
						*/
					}


					//申请一段subTlvlength大小的内存存放该TLV的内容
					temp = (unsigned char *)malloc(subTlvLength);
					memcpy(temp, buffer + currentIndex + 1 + lengthSize, subTlvLength);

				}
				else
				{
					//最高位为0
					subTlvLength = buffer[currentIndex];
					temp = (unsigned char *)malloc(subTlvLength);

					memcpy(temp, buffer + currentIndex + 1, subTlvLength);

				}
				temp[subTlvLength] = 0;

				unsigned int oSize;//输出有多少个同等级的子TLV，解析时也应该用到
								   //不清楚子TLV同等级的TLV有多少个，申请100TLV大小的内存肯定够用
				PTlvEntity[currentTLVIndex].subTLVEntity = (PASCII_TLV)malloc(sizeof(PASCII_TLV[100]));
                ParseAsciiTLV(temp, subTlvLength, PTlvEntity[currentTLVIndex].subTLVEntity, oSize);

				PTlvEntity[currentTLVIndex].subTLVnum = oSize; //填入子TLV的数量

			}
			currentStatus = 'L';
			break;
		case 'L':
			//判断长度字节的最高位是否为1，如果为1，则该字节为长度扩展字节，由下一个字节开始决定长度
			if ((buffer[currentIndex] & 0x80) == 0x80)
			{
				//最高位1
				unsigned int lengthSize = buffer[currentIndex] & 0x7f;
				currentIndex += 1; //从下一个字节开始算Length域
				for (unsigned int index = 0; index < lengthSize; index++)
				{
					valueSize += buffer[currentIndex + index] << ((lengthSize - 1 - index) * 8); //计算Length域的长度
				}
				PTlvEntity[currentTLVIndex].length = (unsigned char *)malloc(lengthSize);
				memcpy(PTlvEntity[currentTLVIndex].length, buffer + currentIndex, lengthSize);
				PTlvEntity[currentTLVIndex].length[lengthSize] = 0;
				PTlvEntity[currentTLVIndex].lengthSize = lengthSize;

				currentIndex += lengthSize;
			}
			else
			{
				//最高位0
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

			//进入下一个TLV构造循环
			currentTLVIndex += 1;

			currentStatus = 'T';
			break;
		}
	}
	entitySize = currentTLVIndex;
}

/****************************************************************
* 判断某段数据是否为TLV结构
*****************************************************************/
bool IsAsciiTlvStruct(unsigned char* buffer, unsigned int bufferLength)
{
    unsigned int currentIndex = 0;							//用于标记buffer
                                                            //判断是Tag是否为多字节,字节的1--5位是否都为1，是的话有后续字节
    while (currentIndex < bufferLength)
    {
        if ((buffer[currentIndex] & 0x1f) == 0x1f)  //tag为多字节
        {
            int endTagIndex = currentIndex;
            while ((buffer[++endTagIndex] & 0x80) == 0x80); //最后一个字节的最高位为0
            int tagSize = endTagIndex - currentIndex + 1; //计算标签所占用字节
            currentIndex += tagSize;
        }
        else       //tag为单字节
        {
            currentIndex += 1;
        }

        int dataLen = 0;
        unsigned int lenSize = 0;
        //计算Length域长度
        if ((buffer[currentIndex] & 0x80) == 0x80)
        {
            //最高位为1
            lenSize = buffer[currentIndex] & 0x7f;
            for (unsigned int index = 0; index < lenSize; index++)
            {
                dataLen += buffer[currentIndex + 1 + index] << ((lenSize - 1 - index) * 8);
            }
            lenSize += 1;
        }
        else  //最高位为0
        {
            dataLen = buffer[currentIndex];
            lenSize = 1;
        }
        currentIndex += lenSize + dataLen;
    }

    return currentIndex == bufferLength;
}