// DpParse_AC.cpp: 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "DpParse_AC.h"
#include "../Util/Des0.h"
#include "../Util/Tool.h"
#include <iostream>


bool HandleDp(const char* szFileName, const char* ruleFile, char** cpsFile, int& count)
{
    ACDpParse parse;
    return parse.HandleDp(szFileName, ruleFile, cpsFile, count);
}

void Decrypt_Des3_CBC(const char* key, const char* input, char* output, int outputLen)
{
    int dataLen = strlen(input);
    int count = dataLen / 16;

    string leftKey = string(key).substr(0, 16);
    string rightKey = string(key).substr(16);
    string result;
    char output1[17] = { 0 };
    string data = input;
    for (int i = count - 1; i >= 0; i--)
    {
        _Des(output1, (char*)leftKey.c_str(), (char*)data.substr(i * 16, 16).c_str());
        Des(output1, (char*)rightKey.c_str(), output1);
        _Des(output1, (char*)leftKey.c_str(), output1);
        if (i != 0)
        {
            char temp[17] = { 0 };
            strncpy_s(temp, 17, data.substr((i - 1) * 16, 16).c_str(), 16);
            str_xor(output1, temp, 16);
        }
        result = output1 + result;
    }
    int len = result.length();
    strncpy_s(output, outputLen, result.c_str(), result.length());
}



bool ACDpParse::HandleDp(const char* szFileName, const char* szRuleFile, char** cpsFile, int& count)
{
    ifstream dpFile;
    if (!OpenDpFile(szFileName, dpFile)) {
        return false;
    }
    //char recordType[2] = { 0 };
    //GetBuffer(dpFile, recordType, 1);
    string recordType;
    GetBCDBuffer(dpFile, recordType, 1);
    //char versionNumber[5] = { 0 };
    string versionNumber;
    GetBCDBuffer(dpFile, versionNumber, 4);
    //char cardDesignTemplate[12] = { 0 };
    string cardDesignTemplate;
    GetBCDBuffer(dpFile, cardDesignTemplate, 10);
    //char creationDate[12] = { 0 };
    string creationDate;
    GetBCDBuffer(dpFile, creationDate, 8);
    //char creationTime[12] = { 0 };
    string creationTime;
    GetBCDBuffer(dpFile, creationTime, 6);
    //char issuerId[4] = { 0 };
    string issuerId;
    GetBCDBuffer(dpFile, issuerId, 3);
    //char remark[128] = { 0 };
    string remark;
    GetBCDBuffer(dpFile, remark, 100);
    //char numberOfCards[7] = { 0 };
    string numberOfCards;
    GetBCDBuffer(dpFile, numberOfCards, 6);
    string encryptedKEK;
    GetBCDBuffer(dpFile, encryptedKEK, 16);
    string kcvEKE;
    GetBCDBuffer(dpFile, kcvEKE, 3);
    string enterChar;
    GetBCDBuffer(dpFile, enterChar, 2);

    char decryptedKEK[33] = { 0 };
    Decrypt_Des3_CBC("52880AA634A018DF99FFE33C94B92B32", encryptedKEK.c_str(), decryptedKEK, 33);
  /*  cout << "=============================== CIF Header ============================" << endl;
    cout << "Record Type: " << recordType << "\t\t" << "Version Number: " << versionNumber << endl;
    cout << "Card Design Template" << cardDesignTemplate << "\t\t" << "CreateDate: " << creationDate << endl;
    cout << "Creation Time" << creationTime << "\t\t" << "Issuer Id: " << issuerId << endl;
    cout << "Remark: " << remark << "\t\t" << "Number of cards: " << numberOfCards << endl;
    cout << "KEK:" << decryptedKEK << "\t\t" << "KEK KCV: " << kcvEKE << endl;
    cout << "=============================== CIF Body ==============================" << endl;*/

    ofstream file("D:\\rdbuf.txt");
    streambuf *x = cout.rdbuf(file.rdbuf());
    while (dpFile)
    {

        GetBCDBuffer(dpFile, recordType, 1);
        //char recordNumber[7] = { 0 };
        string recordNumber;
        GetBCDBuffer(dpFile, recordNumber, 6);
        //char cardNumber[17] = { 0 };
        string cardNumber;
        GetBCDBuffer(dpFile, cardNumber, 16);
        //char cardMapVersion[3] = { 0 };
        string cardMapVersion;
        GetBCDBuffer(dpFile, cardMapVersion, 2);
        //char keyIndex[3] = { 0 };
        string keyIndex;
        GetBCDBuffer(dpFile, keyIndex, 2);
        //char cardType[2] = { 0 };
        string cardType;
        GetBCDBuffer(dpFile, cardType, 1);

 /*       cout << "Record Type: " << recordType << "\t\t" << "Record Number:" << recordNumber << endl;
        cout << "=============================== Card Details ============================" << endl;
        cout << "Card Number: " << cardNumber << "\t\t" << "Card Map Version" << cardMapVersion << endl;
        cout << "Key Index: " << keyIndex << "\t\t" << "Card Type: " << cardType << endl;*/
        cout << "0201: " << cardNumber + cardMapVersion + keyIndex << endl;
        string encryptedMemberDetails;
        GetBCDBuffer(dpFile, encryptedMemberDetails, 120);
        char decryptedMemberDetails[121 * 2] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedMemberDetails.c_str(), decryptedMemberDetails, 121 * 2);
        string memberDetails = decryptedMemberDetails;
        string cardExpiryDate = memberDetails.substr(0, 16);// Tool::BcdToStr(memberDetails.substr(0, 16).c_str());
        string membershipNumber = memberDetails.substr(16, 32);// Tool::BcdToStr(memberDetails.substr(16, 32).c_str());
        string membershipDate = memberDetails.substr(48, 16);// Tool::BcdToStr(memberDetails.substr(48, 16).c_str());
        string membershipName = memberDetails.substr(64, 52);// Tool::BcdToStr(memberDetails.substr(64, 52).c_str());
        string dateOfBirth = memberDetails.substr(116, 16);// Tool::BcdToStr(memberDetails.substr(116, 16).c_str());
        string idNumber = memberDetails.substr(128, 48);//Tool::BcdToStr(memberDetails.substr(128, 48).c_str());
        string companyName = memberDetails.substr(176, 50);//Tool::BcdToStr(memberDetails.substr(176, 50).c_str());

 /*       cout << "================================ Member Details ==========================" << endl;
        cout << "Card Expiry Date: " << cardExpiryDate << "\t\t" << "Membership Number: " << membershipNumber << endl;
        cout << "Membership Date: " << membershipDate << "\t\t" << "Membershit Name: " << membershipName << endl;
        cout << "Date of Birth: " << dateOfBirth << "\t\t" << "ID Number: " << idNumber << endl;
        cout << "Company Name: " << companyName << endl;*/

        cout << "0207: " << cardExpiryDate + membershipNumber + membershipDate + membershipName + dateOfBirth + idNumber + companyName << endl;
        //char loanAmount[13] = { 0 };
        string loanAmount;
        GetBCDBuffer(dpFile, loanAmount, 12);
        //char amountToBeRecovered[13] = { 0 };
        string amountToBeRecovered;
        GetBCDBuffer(dpFile, amountToBeRecovered, 12);
        //char loanStartDate[9] = { 0 };
        string loanStartDate;
        GetBCDBuffer(dpFile, loanStartDate, 8);
        //char loanEndDate[9] = { 0 };
        string loanEndDate;
        GetBCDBuffer(dpFile, loanEndDate, 8);
        //char numberOfInstallments[3] = { 0 };
        string numberOfInstallments;
        GetBCDBuffer(dpFile, numberOfInstallments, 2);
        //char firstInstallmentDate[9] = { 0 };
        string firstInstallmentDate;
        GetBCDBuffer(dpFile, firstInstallmentDate, 8);
        //char installmentAmount[13] = { 0 };
        string installmentAmount;
        GetBCDBuffer(dpFile, installmentAmount, 12);
        //char installmentFrequency[2] = { 0 };
        string installmentFrequency;
        GetBCDBuffer(dpFile, installmentFrequency, 1);

 /*       cout << "================================ Loan Details ==========================" << endl;
        cout << "Loan Amount: " << loanAmount << "\t\t" << "Amount to be recovered: " << amountToBeRecovered << endl;
        cout << "Loan Start Date: " << loanStartDate << "\t\t" << "Loan End Date: " << loanEndDate << endl;
        cout << "No. of installments: " << numberOfInstallments << "\t\t" << "First Installment Date: " << firstInstallmentDate << endl;
        cout << "Installment Amount: " << installmentAmount << "\t\t" << "Installment Frequency: " << installmentFrequency << endl;*/
        cout << "0202: " << loanAmount + amountToBeRecovered + loanStartDate + loanEndDate + numberOfInstallments + firstInstallmentDate + installmentAmount + installmentFrequency << endl;
        //char nextInstallmentDate[9] = { 0 };
        string nextInstallmentDate;
        GetBCDBuffer(dpFile, nextInstallmentDate, 8);
        //char numberOfRemainingInstallments[3] = { 0 };
        string numberOfRemainingInstallments;
        GetBCDBuffer(dpFile, numberOfRemainingInstallments, 2);

        /*cout << "================================ Repayment Details ==========================" << endl;
        cout << "Next Installment Date: " << nextInstallmentDate << "\t\t" << "No of Remaining Installments: " << numberOfRemainingInstallments << endl;*/
        cout << "0203: " << nextInstallmentDate + numberOfRemainingInstallments << endl;

        //char outstandingBalanceAmount[13] = { 0 };
        string outstandingBalanceAmount;
        GetBCDBuffer(dpFile, outstandingBalanceAmount, 12);
        //cout << "================================ Balance Details ==========================" << endl;
        //cout << "Outstanding Balance amount: " << outstandingBalanceAmount << endl;
        cout << "0204: " << outstandingBalanceAmount << endl;

        string encryptedCardSensitiveDataDetails;
        GetBCDBuffer(dpFile, encryptedCardSensitiveDataDetails, 120);
        char decryptedCardSensitiveDataDetails[121 * 2] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedCardSensitiveDataDetails.c_str(), decryptedCardSensitiveDataDetails, 121 * 2);
        string cardSensitiveDataDetails = decryptedCardSensitiveDataDetails;
        int len = cardSensitiveDataDetails.length();
        string iccTrack2Data = cardSensitiveDataDetails.substr(0, 74); // Tool::BcdToStr(cardSensitiveDataDetails.substr(0, 74).c_str());
        string msTrack2Data = cardSensitiveDataDetails.substr(74, 74); // Tool::BcdToStr(cardSensitiveDataDetails.substr(74, 74).c_str());
        string iccTrack2Data2 = cardSensitiveDataDetails.substr(148, 74);// Tool::BcdToStr(cardSensitiveDataDetails.substr(148, 74).c_str());
        string cvv2 = cardSensitiveDataDetails.substr(222, 6); // Tool::BcdToStr(cardSensitiveDataDetails.substr(222, 6).c_str());
        string filler = cardSensitiveDataDetails.substr(228, 12); // Tool::BcdToStr(cardSensitiveDataDetails.substr(228, 12).c_str());

        //cout << "================================ Card Sensitive Data Details ==========================" << endl;
        cout << "021A: " << endl;
        cout << "ICC Track2 Data: " << iccTrack2Data << endl;
        cout << "MS Track2 Data: " << msTrack2Data << endl;
        cout << "ICC Track2 Data: " << iccTrack2Data2 << endl;
        cout << "CVV2: " << cvv2 << endl;
        cout << "Filler: " << filler << endl;

        string cardKeyAndSecretCodeDetails;
        GetBCDBuffer(dpFile, cardKeyAndSecretCodeDetails, 90);

        string encryptedOrsc1SecretCode = cardKeyAndSecretCodeDetails.substr(0, 16);
        char orsc1SecretCode[17] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedOrsc1SecretCode.c_str(), orsc1SecretCode, 17);
        string orsc1PCV = cardKeyAndSecretCodeDetails.substr(16, 6);

        string encryptedOrsc2SecretCode = cardKeyAndSecretCodeDetails.substr(22, 16);
        char orsc2SecretCode[17] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedOrsc2SecretCode.c_str(), orsc2SecretCode, 17);
        string orsc2PCV = cardKeyAndSecretCodeDetails.substr(38, 6);

        string encryptedOrsc3SecretCode = cardKeyAndSecretCodeDetails.substr(44, 16);
        char orsc3SecretCode[17] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedOrsc3SecretCode.c_str(), orsc3SecretCode, 17);
        string orsc3PCV = cardKeyAndSecretCodeDetails.substr(60, 6);

        string encryptedOrencryptionKey = cardKeyAndSecretCodeDetails.substr(66, 32);
        char orencryptionKey[33] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedOrencryptionKey.c_str(), orencryptionKey, 33);
        string orencryptionKCV = cardKeyAndSecretCodeDetails.substr(98, 6);

        string encryptedOrencryptionExtKey = cardKeyAndSecretCodeDetails.substr(104, 32);
        char orencryptionExtKey[33] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedOrencryptionExtKey.c_str(), orencryptionExtKey, 33);
        string orencryptionExtKCV = cardKeyAndSecretCodeDetails.substr(136, 6);

        string encryptedOrencryptionIntKey = cardKeyAndSecretCodeDetails.substr(142, 32);
        char orencryptionIntKey[33] = { 0 };
        Decrypt_Des3_CBC(decryptedKEK, encryptedOrencryptionIntKey.c_str(), orencryptionIntKey, 33);
        string orencryptionIntKCV = cardKeyAndSecretCodeDetails.substr(174, 6);

        //cout << "================================ Card Keys and Secret Code Details ==========================" << endl;
        cout << "ORSC1 Secret Code: " << orsc1SecretCode << "\t\t" << "ORSC1 PCV: " << orsc1PCV << endl;
        cout << "ORSC2 Secret Code: " << orsc2SecretCode << "\t\t" << "ORSC2 PCV: " << orsc2PCV << endl;
        cout << "ORSC3 Secret Code: " << orsc3SecretCode << "\t\t" << "ORSC3 PCV: " << orsc3PCV << endl;

        cout << "ORENCRYPTION KEY: " << orencryptionKey << "\t\t" << "KCV: " << orencryptionKCV << endl;
        cout << "ORENCRYPTION EXT KEY: " << orencryptionExtKey << "\t\t" << "KCV: " << orencryptionExtKCV << endl;
        cout << "ORENCRYPTION INT KEY: " << orencryptionIntKey << "\t\t" << "KCV: " << orencryptionIntKCV << endl;

        string changeLine;
        GetBCDBuffer(dpFile, changeLine, 2);

    }
    cout.rdbuf(x);
    return true;
}
