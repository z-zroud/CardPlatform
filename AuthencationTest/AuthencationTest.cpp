// AuthencationTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Authencation/IGenKey.h"

int main()
{
    string dgi0201 = "EHFIWHEF5F340101shdifewh";
    int pos = dgi0201.find("5F34");
    int len = stoi(dgi0201.substr(pos + 4, 2), 0, 16);

    string value = dgi0201.substr(pos, len * 2 + 4);


    string testStr = "49ABC12346493534uDEF3943ABCDEFw93";
    int startIndex = testStr.find("ABC");
    int endIndex = testStr.find("DEF");
    auto test = testStr.find("49", startIndex);

    string issuerFlag = string("1234567890").substr(2, 8);
    int index = issuerFlag.find('F');
    string leftPAN = issuerFlag.substr(0, index);
    if (string("34567890").substr(0, index) != leftPAN)
    {
        return 2;   //issuer mark error
    }


    //Test kcv
    char* smKey = "EF157139EF975D3BB00DB8D27CA02090";
    char* desKey = "9EE66BE0F25DA4D308978CDF643BD00D";

    char kcv[7] = { 0 };
    GenDesKcv(desKey, kcv, 6);

    GenSmKcv(smKey, kcv, 6);
    char udk[33] = { 0 };
    char udkSessionKey[33] = { 0 };
    GenUdk("9EE66BE0F25DA4D308978CDF643BD00D", "6270420226000000014F", "00", udk);
    GenUdk("6142CD7233762A2293D34A74F763BC28", "6270420226000000014F", "00", udk,1);
    GenUdkSessionKey(udk, "0036", udkSessionKey, 1);
    GenUdkSessionKey("DBD2EB08989239C98EC250696B5EF1F8", "0036", udkSessionKey, 1);
    char arpc[17] = { 0 };
    char ac[17] = "88E307B07FDAA4E4";
    GenArpc(udkSessionKey, ac, "3030", arpc, 1);
	//Test GenCAPublicKey interface
	char caPublicKey[2048] = { 0 };
	GenCAPublicKey("0B","A000000333", caPublicKey);

	printf("CA Publick Key: %s\n", caPublicKey);

	//Test DES GenDesIssuerPublicKey
	const char issuerPublicCert[] = "4E4261144DEC743104199E69CCEFEDD4160C076C41DD9599ADD3620468E02A95CDB2560FB2FE2A275B705F5D8D3E56C9615B7B9DF985B6120E10C8160CCED1980DD94B0FE8BCB2167511E5A32E472BF02F31D40F50A44D1CC1F7AF8FC8B7A13CC9CA1841AB82540D939CF334554E17CA0CFF4284F3D6448CBD2FF17E3AEAFC2FC389EE973631C8B965F168FF4A01379244CDB805AF715C5CF0E40638051D6B64A2D0A925644F8A4B5EDB498F749AAFA5FF060A4B825EB80AA13851810F5D3E51BFE2BD025234BB1D2842DE3A4B6E633C80095CD13DA0847FC0D0262D55A9448DE568C2FA7E4D46619F72E11DA5867CE5D490258B27EE0558";
	const char* ipkRemainder = NULL;
	const char issuerExponent[] = "03";
	char issuerPublicKey[2048] = { 0 };
	GenDesIssuerPublicKey(caPublicKey, issuerPublicCert, ipkRemainder, issuerExponent, issuerPublicKey);
	printf("Issuer Public Key: %s\n", issuerPublicKey);

	//Test DES GenDesICCPublicKey
	const char* iccPublicCert = "7BEA4A79248FEB1294B5FDA2462CFA92AF3F300FDE897CB76F04EE10D3D1385CB54F59EE8C756474D7A34ED4919785CE4662A188F5F39E844E8A11C5E1982E111670FD48E75387B439E5297A31EBC73F203DFF77E8E11246AA509A0CF78403C13D2099253BC050752D88975530C8DE3A901EAA8AAAECB979032DCE16A982BCD259C0EB481D0AAD08CBBA830FA09D7DFC0F64A4CC20A5EEF6CCF70C974EF8FA228E0C3B7F8240C8228319119FFD663A5D";
	const char* iccRemainder = "76E41056372A6AF39D55";
	const char* signedData = "5F24032506015F25031706015A096235489730000218129F0702FF008E0C000000000000000002031F009F0D05D8609CA8009F0E0500100000009F0F05D8689CF8005F280201569F080200308C1B9F02069F03069F1A0295055F2A029A039C019F37049F21039F4E148D1A8A029F02069F03069F1A0295055F2A029A039C019F37049F2103";
	const char* iccExponent = "03";
	char iccPublicKey[2048] = { 0 };
	GenDesICCPublicKey(issuerPublicKey, iccPublicCert, iccRemainder, signedData, iccExponent, "7C00", iccPublicKey);
	printf("ICC Public Key: %s\n", iccPublicKey);

	//Test SM GenSMIssuerPublicKey
	char smCAPublicKey[2048] = { 0 };
	GenCAPublicKey("18", "A000000333", smCAPublicKey);
	printf("SM CA Public Key: %s\n", smCAPublicKey);

	const char* smIssuerPublicCert = "1262354897123000125804001140C9DAD9AC482C02D489E24EEDFCE7C2F0EA4450D208E8E3847FDDCE6B9D55EDC8C97154E019C9E5C1419629587C9C923529D5A18D1715E6EF9DAEB057987FEDFAE1F1F48346B2E90A4443CB7DD42748236C2264FCE4A47B68EF505F2E2FCE4736BA8EC9B67611DB07E31FA90E245300BA278EBDD345508FACE2485E242CE09363";
	char smIssuerPublicKey[2048] = { 0 };
	//GenSMIssuerPublicKey(smCAPublicKey, smIssuerPublicCert, smIssuerPublicKey);
	printf("\nSM Issuer Public Key: %s\n", smIssuerPublicKey);

	//Test SM GenSMICCPublicKey
	const char* smIccPublicCert = "14623548973000021812FF0625007EB004001140342A63B591135AEFC30A2A308A0F8185FB6E35A85F75A26487501B0CAFE391113BEDDA0888B126FC7F64E2E070F9CC06D456BF7C984EE7D1194F9DE23D9ADCA3C4D73FA5EA6B7040AACE51D5AE016DA2DD74F7147BFD31925EFE8C09E5308C26BD54FFE78B8CEEBF140E7506B9418B209C4D244F799CC8B6E3E0FA2F3212DC22";
	const char* needAuthStaticData = "5F24032506015F25031706015A096235489730000218129F0702FF008E0C000000000000000002031F009F0D05D8609CA8009F0E0500100000009F0F05D8689CF8005F280201569F080200308C1B9F02069F03069F1A0295055F2A029A039C019F37049F21039F4E148D1A8A029F02069F03069F1A0295055F2A029A039C019F37049F21037C00";
	char smIccPublicKey[2048] = { 0 };
	GenSMICCPublicKey(smIssuerPublicKey, smIccPublicCert, needAuthStaticData, "7C00","62355358600002171F", smIccPublicKey);
	printf("\nSM ICC Public Key: %s\n", smIccPublicKey);

    return 0;
}

