// TripleDES.h: interface for the TripleDES class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIPLEDES_H__055728AB_6544_4EF8_924E_4EA34CA0F99B__INCLUDED_)
#define AFX_TRIPLEDES_H__055728AB_6544_4EF8_924E_4EA34CA0F99B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TripleDES  
{
public:
	TripleDES();
	virtual ~TripleDES();


	void CopyBit2(unsigned char *des,int startofdes,unsigned char *src,int startofsrc,int len);
	void Char_Bit(unsigned char *Src,__int16 CharLen,unsigned char *Res);
	void Gen_Childkey(unsigned char Key[8],unsigned char Childkey[16][48]);
	void Bit_Char(unsigned char *Src,__int16 BitLen,unsigned char *Res);
	//void TrimSpace(CString &,unsigned char *src);
	void InvertByte(unsigned char *src,int len,unsigned char *des);		/////////////////////////
	void CharToInt(unsigned int *des, UCHAR *src, int pair_len);		/////////////////////////////
	void IntToChar(unsigned char*des,unsigned long*src,int);
	void HexToChar(unsigned char *Dest, unsigned char *Src, unsigned short SrcLen);
	void CharToHex(char *hh, BYTE *dd,int len);
	void MAC0(BYTE *data, int length, BYTE *key, BYTE * random, BYTE *mac);
	void des0(int argc, BYTE *src, BYTE *key, BYTE *result);
	void des_algo(BYTE *src, BYTE *dst, BYTE *key, int flag);
	void transfer(BYTE *src, BYTE *dst, BYTE *table);
	void str_xor(BYTE *src, BYTE *dst, int len);
	void str_cpy(BYTE *src, BYTE *dst, int len);
	void S_change(BYTE *str);
	void key_generate(BYTE *key_in, BYTE *key_sub, int cnt);
	void circle(BYTE *str, int leng);	// to complete left circel shift
	void dtob(BYTE dd,BYTE *hh);

	void CharToByteTest(BYTE * src, BYTE * Dest, BYTE * dst, BYTE * result);
	void CharToByteTest1(BYTE * src, BYTE * Dest, BYTE * dst, BYTE * result);

	unsigned long DES_Encrypt( unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned long SrcLen, unsigned char *result);
	__int16 DES_Decrypt( unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned long SrcLen, unsigned char *result);

	void TriDesEncrypt(unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned short SrcLen, unsigned char *result);
	void TriDesDecrypt(unsigned char *Key,__int16 KeyLen, unsigned char *Source, unsigned short SrcLen, unsigned char *result);
	
	void CalMAC(unsigned char* pszKey, int iKeyLen, unsigned char* pszVector, unsigned char* pszSrc, int iSrcLen, unsigned char* pszMAC);


	// PBOC key diversite
	void KeyDiv(unsigned char* pszKey, unsigned char* pszDiv, unsigned char* pszSubKey);
};

#endif // !defined(AFX_TRIPLEDES_H__055728AB_6544_4EF8_924E_4EA34CA0F99B__INCLUDED_)
