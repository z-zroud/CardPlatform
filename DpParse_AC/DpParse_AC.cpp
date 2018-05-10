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

void Decrypt_Des3_CBC( const char* key,const char* input, char* output, int outputLen)
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
    char recordType[2] = { 0 };
    GetBuffer(dpFile, recordType, 1);
    char versionNumber[5] = { 0 };
    GetBuffer(dpFile, versionNumber, 4);
    char cardDesignTemplate[12] = { 0 };
    GetBuffer(dpFile, cardDesignTemplate, 10);
    char creationDate[12] = { 0 };
    GetBuffer(dpFile, creationDate, 8);
    char creationTime[12] = { 0 };
    GetBuffer(dpFile, creationTime, 6);
    char issuerId[4] = { 0 };
    GetBuffer(dpFile, issuerId, 3);
    char remark[128] = { 0 };
    GetBuffer(dpFile, remark, 100);
    char numberOfCards[7] = { 0 };
    GetBuffer(dpFile, numberOfCards, 6);
    string encryptedKEK;
    GetBCDBuffer(dpFile, encryptedKEK, 16);
    string kcvEKE;
    GetBCDBuffer(dpFile, kcvEKE, 3);
    string enterChar;
    GetBCDBuffer(dpFile, enterChar, 2);

    char decryptedKEK[33] = { 0 };
    Decrypt_Des3_CBC("4282C9A950AA99361271D866C541E7F6", encryptedKEK.c_str(), decryptedKEK, 33);

    cout << "=============================== CIF Header ============================" << endl;
    cout << "Record Type: " << recordType << "\t\t" << "Version Number: " << versionNumber << endl;
    cout << "Card Design Template" << cardDesignTemplate << "\t\t" << "CreateDate: " << creationDate << endl;
    cout << "Creation Time" << creationTime << "\t\t" << "Issuer Id: " << issuerId << endl;
    cout << "Remark: " << remark << "\t\t" << "Number of cards: " << numberOfCards << endl;
    cout << "KEK:" << decryptedKEK << "\t\t" << "KEK KCV: " << kcvEKE << endl;
    cout << "=============================== CIF Body ==============================" << endl;
    GetBuffer(dpFile, recordType, 1);
    char recordNumber[7] = { 0 };
    GetBuffer(dpFile, recordNumber, 6);
    char cardNumber[17] = { 0 };
    GetBuffer(dpFile, cardNumber, 16);
    char cardMapVersion[3] = { 0 };
    GetBuffer(dpFile, cardMapVersion, 2);
    char keyIndex[3] = { 0 };
    GetBuffer(dpFile, keyIndex, 2);
    char cardType[2] = { 0 };
    streampos pos = GetBuffer(dpFile, cardType, 1);

    cout << "Record Type: " << recordType << "\t\t" << "Record Number:" << recordNumber << endl;
    cout << "=============================== Card Details ============================" << endl;
    cout << "Card Number: " << cardNumber << "\t\t" << "Card Map Version" << cardMapVersion << endl;
    cout << "Key Index: " << keyIndex << "\t\t" << "Card Type: " << cardType << endl;

    string encryptedMemberDetails;
    GetBCDBuffer(dpFile, encryptedMemberDetails, 120);
    char decryptedMemberDetails[121 * 2] = { 0 };
    Decrypt_Des3_CBC(decryptedKEK, encryptedMemberDetails.c_str(), decryptedMemberDetails, 121 * 2);
    string memberDetails = decryptedMemberDetails;
    string cardExpiryDate = Tool::BcdToStr(memberDetails.substr(0, 16).c_str());
    string membershipNumber = Tool::BcdToStr(memberDetails.substr(16, 32).c_str());
    string membershipDate = Tool::BcdToStr(memberDetails.substr(48, 16).c_str());
    string membershipName = Tool::BcdToStr(memberDetails.substr(64, 52).c_str());
    string dateOfBirth = Tool::BcdToStr(memberDetails.substr(116, 16).c_str());
    string idNumber = Tool::BcdToStr(memberDetails.substr(128, 48).c_str());
    string companyName = Tool::BcdToStr(memberDetails.substr(176, 50).c_str());
    
    cout << "================================ Member Details ==========================" << endl;
    cout << "Card Expiry Date: " << cardExpiryDate << "\t\t" << "Membership Number: " << membershipNumber << endl;
    cout << "Membership Date: " << membershipDate << "\t\t" << "Membershit Name: " << membershipName << endl;
    cout << "Date of Birth: " << dateOfBirth << "\t\t" << "ID Number: " << idNumber << endl;
    cout << "Company Name: " << companyName << endl;

    //read reseved data 5 bytes
    //string reserved;
    //GetBCDBuffer(dpFile, reserved, 5);

    char loanAmount[13] = { 0 };
    GetBuffer(dpFile, loanAmount, 12);
    char amountToBeRecovered[13] = { 0 };
    GetBuffer(dpFile, amountToBeRecovered, 12);
    char loanStartDate[9] = { 0 };
    GetBuffer(dpFile, loanStartDate, 8);
    char loanEndDate[9] = { 0 };
    GetBuffer(dpFile, loanEndDate, 8);
    char numberOfInstallments[3] = { 0 };
    GetBuffer(dpFile, numberOfInstallments, 2);
    char firstInstallmentDate[9] = { 0 };
    GetBuffer(dpFile, firstInstallmentDate, 8);
    char installmentAmount[13] = { 0 };
    GetBuffer(dpFile, installmentAmount, 12);
    char installmentFrequency[2] = { 0 };
    GetBuffer(dpFile, installmentFrequency, 1);

    cout << "================================ Loan Details ==========================" << endl;
    cout << "Loan Amount: " << loanAmount << "\t\t" << "Amount to be recovered: " << amountToBeRecovered << endl;
    cout << "Loan Start Date: " << loanStartDate << "\t\t" << "Loan End Date: " << loanEndDate << endl;
    cout << "No. of installments: " << numberOfInstallments << "\t\t" << "First Installment Date: " << firstInstallmentDate << endl;
    cout << "Installment Amount: " << installmentAmount << "\t\t" << "Installment Frequency: " << installmentFrequency << endl;

    char nextInstallmentDate[9] = { 0 };
    GetBuffer(dpFile, nextInstallmentDate, 8);
    char numberOfRemainingInstallments[3] = { 0 };
    GetBuffer(dpFile, numberOfRemainingInstallments, 2);

    cout << "================================ Repayment Details ==========================" << endl;
    cout << "Next Installment Date: " << nextInstallmentDate << "\t\t" << "No of Remaining Installments: " << numberOfRemainingInstallments << endl;


    char outstandingBalanceAmount[13] = { 0 };
    GetBuffer(dpFile, outstandingBalanceAmount, 12);
    cout << "================================ Balance Details ==========================" << endl;
    cout << "Outstanding Balance amount: " << outstandingBalanceAmount << endl;

    string encryptedCardSensitiveDataDetails;
    GetBCDBuffer(dpFile, encryptedCardSensitiveDataDetails, 120);
    char decryptedCardSensitiveDataDetails[121 * 2] = { 0 };
    Decrypt_Des3_CBC(decryptedKEK, encryptedCardSensitiveDataDetails.c_str(), decryptedCardSensitiveDataDetails, 121 * 2);
    string cardSensitiveDataDetails = decryptedCardSensitiveDataDetails;
    int len = cardSensitiveDataDetails.length();
    string iccTrack2Data = Tool::BcdToStr(cardSensitiveDataDetails.substr(0, 74).c_str());
    string msTrack2Data = Tool::BcdToStr(cardSensitiveDataDetails.substr(74, 74).c_str());
    string iccTrack2Data2 = Tool::BcdToStr(cardSensitiveDataDetails.substr(148, 74).c_str());
    string cvv2 = Tool::BcdToStr(cardSensitiveDataDetails.substr(222, 6).c_str());
    string filler = Tool::BcdToStr(cardSensitiveDataDetails.substr(228, 12).c_str());

    cout << "================================ Card Sensitive Data Details ==========================" << endl;
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

    cout << "================================ Card Keys and Secret Code Details ==========================" << endl;
    cout << "ORSC1 Secret Code: " << orsc1SecretCode << "\t\t" << "ORSC1 PCV: " << orsc1PCV << endl;
    cout << "ORSC2 Secret Code: " << orsc2SecretCode << "\t\t" << "ORSC2 PCV: " << orsc2PCV << endl;
    cout << "ORSC3 Secret Code: " << orsc3SecretCode << "\t\t" << "ORSC3 PCV: " << orsc3PCV << endl;

    cout << "ORENCRYPTION KEY: " << orencryptionKey << "\t\t" << "KCV: " << orencryptionKCV << endl;
    cout << "ORENCRYPTION EXT KEY: " << orencryptionExtKey << "\t\t" << "KCV: " << orencryptionExtKCV << endl;
    cout << "ORENCRYPTION INT KEY: " << orencryptionIntKey << "\t\t" << "KCV: " << orencryptionIntKCV << endl;
    return true;
}
