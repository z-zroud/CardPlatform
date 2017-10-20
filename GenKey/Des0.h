#ifndef _DES0_H
#define _DES0_H

#ifdef __cplusplus
extern "C"
{
#endif
void Common_MAC(char *IN1, char *KEY1, char *initial_vector,char *Mac);
//DES CBC for (n-1) block and 3DES CBC for the last block, auto 80 00 ... 00 if the length of indata is not equeal to 8*n,
//return 8 bytes MAC

void VSDC_MAC(char *indata, char *key, char *Mac);
//FULL 3DES CBC mode Mac,Auto padding 80 00 ... 00 if the length of indata is not equeal to 8*n

void DES_3DES_CBC_MAC(char *indata, char *key, char *initial_vector,char *Mac);
//Same as Common_MAC

void Full_3DES_CBC_MAC(char *indata, char *key, char *initial_vector,char *Mac);
//Same as VSDC_MAC but add 'initial_vector'

void Des3_ECB(char *out, char *key, char *input, unsigned char len);
//Yi := ALG(Ks)[Xi],len=strlen(input)/2; auto padding 80 00 ... 00 if the length of indata is not equeal to 8*n

void Des3_CBC(char *OUT1, char *KEY1, char *IN1, unsigned char len);
//Yi := 3DES(Ks)[Xi��Yi-1],len=strlen(input)/2; auto padding 80 00 ... 00 if the length of indata is not equeal to 8*n

void Des(char *out,char *key,char *input);
//DES Encryption

void Des3(char *out,char *key,char *input);
//3DES Encryption

void _Des(char *out,char *key,char *input);
//DES Decryption

void _Des3(char *out,char *key,char *input);
//3DES Decryption

void AscToBcd(unsigned char *bcd,unsigned char *asc,long asc_len);
//Convert ASCII string to BCD, "3132.."==>"12..", len<256

void BcdToAsc(unsigned char *bcd,char *asc,long asc_len);
//Convert BCD code to ASCII string, "12.."==>"3132..", len<256

void data_xor_char(char *out,char *Datain,unsigned char len);
//���һ��char�͵��ַ������õ�һ��char�͵����ֵ
//����������

void str_xor(char *Datain1,char *Datain2, int len);
//�����ַ�����򣬽����Datain1��
//������Կ�������

int GenRSAKey_STD(int bits, char *pE, char *pN,  char *pD);
//����RSA��Կ�ԣ�STD mode
//���룺bits-��Կģ����(λ)��pE����Կָ����
//�����pN��ģ��pD��˽Կָ��
//����ֵ��0���ɹ�������ֵ��ʧ��

int GenRSAKey_CRT(int key_bits_in, char *PublicExponent_in, char *Modulus_out,  char *Prime1_out, char *Prime2_out , char *PrivateExponent1_out , char *PrivateExponent2_out ,char *InversePrime2_out);
//����RSA��Կ�ԣ�CRT mode
//���룺key_bits_in-��Կģ����(λ)��PublicExponent_in����Կָ����
//�����Modulus_out��ģ��
//RSA 5�����֣�Prime1_out, Prime2_out , PrivateExponent1_out , PrivateExponent2_out , InversePrime2_out
//����ֵ��0���ɹ�������ֵ��ʧ��

int GenRSAKey_CRT_STD(int key_bits_in, char *PublicExponent_in, char *PrivateExponent_out, char *Modulus_out,  char *Prime1_out, char *Prime2_out , char *PrivateExponent1_out , char *PrivateExponent2_out ,char *InversePrime2_out);
//����RSA��Կ�ԣ�CRT & STD mode
//���룺key_bits_in-��Կģ����(λ)��PublicExponent_in����Կָ����
//�����Modulus_out��ģ��PrivateExponent_out��˽Կָ��
//RSA 5�����֣�Prime1_out, Prime2_out , PrivateExponent1_out , PrivateExponent2_out , InversePrime2_out
//����ֵ��0���ɹ�������ֵ��ʧ��

int RSA_STD(char *pN, char *pE, char *pData, char *pOUT);
//RSA STD mode��
//���룺pN��ģ��pE����Կָ����pData����������
//�����pOUT�����ܽ��
//����ֵ��0���ɹ�������ֵ��ʧ��

int RSA_CRT(char *Data_in,char *Prime1_in,char *Prime2_in, char *PrivateExponent1_in, char *PrivateExponent2_in, char *InversePrime2_in,char *res_out);
//RSA CRT mode��˽Կǩ��/����
//���룺Data_in���������ݣ�
//		(RSA Key 5������)Prime1_out, Prime2_out , PrivateExponent1_out , PrivateExponent2_out , InversePrime2_out
//�����res_out��˽Կ����/ǩ�����
//����ֵ��0���ɹ�������ֵ��ʧ��

//RSA STD mode ��Կ����,��ͬ��RSA_STD����
void RSA_Encrypt(char *pN, char *pE, char *pData,short dl, char *pOUT );

//RSA STD mode ˽Կ����,��ͬ��RSA_STD����
void RSA_Decrypt(char *pN, char *pD, char *pData,short dl, char *pOUT );

//RSA ǩ��
void RSA_Sign( char *pN, char *pD, char *hash, short hash_len, char *pSign);

//Convert an ASCII hex string to a number and specify how many ASCII digits to be converted, e.g.: "FFFF8888" ==> 0xFFFF8888
long AscToLen_n(char *asc, int len2convert);

//Convert an ASCII hex string to a number, e.g.: "FFFF8888" ==> 0xFFFF8888
long AscToLen(char *asc);

//Convert a hex number to a ASCII hex char, e.g.: 0xF ==> 'F'
char HexToChar(unsigned char hex);

//Convert a ASCII hex char to a hex number, e.g.: 'F'==>0xF
unsigned char AscToHex(char asc);

#ifdef __cplusplus
}
#endif
/* End of extern "C" { */
#endif  /* __cplusplus */