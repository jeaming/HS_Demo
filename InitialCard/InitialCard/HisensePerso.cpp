/*****************************************************************************
**
**			This file contain hisense USER/AGENCY/AMDIN Card perso procedure
**			It also include "PSAM" CARD perso procedure
**
**							By Jason 5/5/2018
******************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "InitialCard_HZDlg.h"

//Key defination

//these two will not devers
#define TK	(unsigned char*)"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
#define DCCK	(unsigned char*)"\xF4\x23\x59\xA1\xAF\x4D\x15\x8A\x67\xE8\xFD\x2D\x15\x0E\x26\xF7"//0


#define CMK	(unsigned char*)"\x1A\xE5\x7A\x56\xBA\x58\x0C\x12\x4D\x40\xE8\xA7\xC7\x12\x51\x65"//1
#define EAK	(unsigned char*)"\x10\x97\xB5\xC8\xBE\x0E\x43\xE4\x53\x24\x63\x1D\x4F\xA4\x44\x27"//2
#define PUK	(unsigned char*)"\x1E\xA3\x5C\x35\xAD\x61\xDF\xD2\xD7\x25\x46\xEB\x28\xA4\x3F\x70"//3
#define PRK	(unsigned char*)"\x1E\xAE\xB3\x7A\x21\xB9\xDE\xA2\xA2\x9E\x39\x0E\x44\x97\x38\xB8"//4
#define PIN	(unsigned char*)"\x12\x34\x56\xFF\xFF\xFF\xFF\xFF"

#define AMK	(unsigned char*)"\xDF\x1A\x09\x56\x51\x83\x40\xB4\x53\xA4\x49\x2A\x00\xB7\xEA\x2F"//5
#define APUK	(unsigned char*)"\xE1\x6E\xCB\xD7\x90\x2D\xF6\xAA\x7A\x70\xD5\x71\xF8\x0D\x2D\xE6"//6
#define APRK	(unsigned char*)"\xE8\x6B\x00\x5A\x61\x6D\x67\xB3\x5A\xD6\x49\xEC\xB4\xA1\x9D\x82"//7
#define APK1	(unsigned char*)"\x17\xE6\x7D\xC5\x0B\x85\x78\xAD\xFE\xA8\x8B\x2B\x43\x5C\x9A\xDE"//8
#define APK2	(unsigned char*)"\x5F\x9F\x18\x0F\x23\xDC\xD8\x4C\xAD\xFF\xDE\x39\x97\xF3\x67\xB1"//9
#define ALK	(unsigned char*)"\xF4\x48\x2A\xE9\xE0\x8D\x8B\xA0\x34\xE9\x97\x22\x74\xC6\xEE\x71"//10
#define ATK	(unsigned char*)"\xCB\x7D\x0C\xD3\x98\xD3\x78\xEE\x17\xF9\x91\xBA\x9D\xE5\x54\x21" //11
#define EK	(unsigned char*)"\x71\x3C\x52\xAE\xA9\xE2\x0A\xBC\x5F\xC0\x15\xB8\xA5\xB8\x95\xC2"//12
//ACK

enum KEY_SETS
{
	DCMK =1,
	DEAK,
	DPUK,
	DPRK,
	DAMK,
	DAPUK,
	DAPRK,
	DAPK1,
	DAPK2,
	DALK,
	DATK,
	DEK,
	DACK
};


//diversify keys
int CInitialCard_HZDlg::DiversKeys()
{
	unsigned char szDIV[9];

	memset(szDIV, 0, sizeof(szDIV));
	m_pcscOp.Hex2Bin(szDIV, m_szDIV, 16);

	for (int i = 0; i != KEY_COUNT; ++i)
	{
		memset(m_szCardKeys[i], 0, sizeof(m_szCardKeys[i]));
	}

	m_pcscOp.Bin2Hex(m_szCardKeys[0], DCCK, 16);

	//dcmk
	des.KeyDiv(CMK, szDIV, m_szCardKeys[DCMK]);

	//EAK
	des.KeyDiv(EAK, szDIV, m_szCardKeys[DEAK]);

	//PUK
	des.KeyDiv(PUK, szDIV, m_szCardKeys[DPUK]);

	//PRK
	des.KeyDiv(PRK, szDIV, m_szCardKeys[DPRK]);

	//AMK
	des.KeyDiv(AMK, szDIV, m_szCardKeys[DAMK]);

	//APUK
	des.KeyDiv(APUK, szDIV, m_szCardKeys[DAPUK]);

	//APRK
	des.KeyDiv(APRK, szDIV, m_szCardKeys[DAPRK]);

	//APK1
	des.KeyDiv(APK1, szDIV, m_szCardKeys[DAPK1]);

	//APK2
	des.KeyDiv(APK2, szDIV, m_szCardKeys[DAPK2]);

	//ALK
	des.KeyDiv(ALK, szDIV, m_szCardKeys[DALK]);

	//ATK
	des.KeyDiv(ATK, szDIV, m_szCardKeys[DATK]);

	//DEK
	des.KeyDiv(EK, szDIV, m_szCardKeys[DEK]);

	//ACK
	des.KeyDiv(DCCK, szDIV, m_szCardKeys[DACK]);

	return 0;
}

//initialize user card MF
int CInitialCard_HZDlg::InitializeMF()
{
	int iRet = 0;
	unsigned char szRetSW[5], szRetData[550];

	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"00A40000023F00", 14, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("选择主目录失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	if (!memcmp(szRetSW, "6A82", 4))//无主目录
	{
		//create file
		memset(szRetSW, 0, sizeof(szRetSW));
		memset(szRetData, 0, sizeof(szRetData));
		iRet = ICC_SendCommand((unsigned char*)"80E03F000D38FFFFAAAAFFFFFFFFFFFFFFFF", 36, szRetSW, szRetData);
		if (iRet < 0 || iRet > 255)
		{
			DisplayHint("建立主目录失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
			return 4;
		}

	}
	else
	{//with MF
		iRet = ExternalAuth_MK(DCCK, 16);
		if (iRet)
		{
			iRet = ExternalAuth_MK(TK, 16);
			if (iRet)
				return iRet;

		}

		//清卡
		memset(szRetSW, 0, sizeof(szRetSW));
		memset(szRetData, 0, sizeof(szRetData));
		iRet = ICC_SendCommand((unsigned char*)"800E0000", 8, szRetSW, szRetData);
		if (iRet < 0 || iRet > 255)
		{
			DisplayHint("清卡失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
			return 4;
		}

	}

	//create file Key File
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"80E00000073F00EB01AAFFFF", 24, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("建立密钥文件失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//update Key
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"80D401001539F0AA0A66F42359A1AF4D158A67E8FD2D150E26F7", 52, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("写入主控密钥失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}


	return 0;
}

//write USER card keys & card number
int CInitialCard_HZDlg::WriteUserCardKeys()
{
	int iRet = 0, iLen = 0;
	unsigned char szRetSW[5], szRetData[520];
	unsigned char szCmd[128];

	iRet = DiversKeys();
	if (iRet)
		return iRet;


	//select MF
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"00A40000023F00", 14, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("选择主目录失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	iRet = ExternalAuth_MK(DCCK, 16);
	if (iRet)
		return iRet;

	//DEAK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 14;
	memcpy(szCmd, "00A4000002EF05", iLen);
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("选择EF05文件失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//Update card number
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 10;
	memcpy(szCmd, "00D600000B", 10);
	switch (m_cmbPersoType.GetCurSel())
	{
	case ADMIN_CARD:memcpy(szCmd + iLen, "00", 2); break;
	case SINGLE_CAR_CARD:memcpy(szCmd + iLen, "01", 2); break;
	case AGENCY_CARD:memcpy(szCmd + iLen, "02", 2); break;
	default:{DisplayHint("卡类别选择错误(%d)!", m_cmbPersoType.GetCurSel()); return 4; }
		break;
	}
	iLen += 2;
	strTemp.Empty();
	m_EdtSnBegin.GetWindowTextA(strTemp);
	m_pcscOp.Bin2Hex(szCmd + iLen, (unsigned char*)strTemp.GetBuffer(), 10);
	iLen += 20;

	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("更新卡号失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DEAK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401011539F0AA0766",iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DEAK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DEAK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DCMK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401011536F0FAFF66", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DCMK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DCMK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DPUK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401001537F0FAFF66", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DPUK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DPUK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DPRK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401001538F0FAFF66", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DPRK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DPUK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	if (m_cmbPersoType.GetCurSel() == ADMIN_CARD)
	{//dek
		//DEK
		memset(szRetSW, 0, sizeof(szRetSW));
		memset(szRetData, 0, sizeof(szRetData));
		memset(szCmd, 0, sizeof(szCmd));

		iLen = 20;
		memcpy(szCmd, "80D401011530F0FAFF66", iLen);
		m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DEK], 16);
		iLen += 32;
		iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
		if (iRet < 0 || iRet > 255)
		{
			DisplayHint("安装DPUK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
			return 4;
		}

	}

	////////////////////////////////////////////////////	DF01
	//select MF
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"00A4000002DF01", 14, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("选择DF目录失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	iRet = ExternalAuth_MK(TK, 16);
	if (iRet)
	{
		DisplayHint("认证DF主控失败;");
		return iRet;
	}

	//DACK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 10;
	memcpy(szCmd, "80D4390010", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DACK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DACK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DEAK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401011539F0AA0766", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DEAK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DEAK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DAMK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401011536F0FAFF66", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DAMK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DAMK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DAPUK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401001537F0FAFF66", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DAPUK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DPUK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DAPRK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401001538F0FAFF66", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DAPRK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DAPRK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DAPK1
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D40101153EF0FA0100", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DAPK1], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DAPK1失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DAPK2
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D40102153EF0FA0200", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DAPK2], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DAPK2失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DALK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D40101153FF0FA0100", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DALK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DALK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//DATK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 20;
	memcpy(szCmd, "80D401011534F0FA0100", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DATK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DATK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	return 0;
}

//write PSAM card keys/card No
int CInitialCard_HZDlg::WritePSAMCardKeys()
{
	int iRet = 0, iLen = 0;
	unsigned char szRetSW[5], szRetData[520];
	unsigned char szCmd[128];
	unsigned char szDIV[9];

	memset(szDIV, 0, sizeof(szDIV));
	m_pcscOp.Hex2Bin(szDIV, m_szDIV, 16);

	//DEK
	memset(m_szCardKeys[DEK], 0, sizeof(m_szCardKeys[DEK]));
	des.KeyDiv(EK, szDIV, m_szCardKeys[DEK]);

	//select MF
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"00A40000023F00", 14, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("选择主目录失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	iRet = ExternalAuth_MK(DCCK, 12);
	if (iRet)
		return iRet;

	//Update terminal ID
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 12;
	memcpy(szCmd, "00D696000600", iLen);

	strTemp.Empty();
	m_EdtSnBegin.GetWindowTextA(strTemp);
	memcpy(szCmd + iLen, strTemp.GetBuffer(), 10);
	iLen += 10;

	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("更新终端号失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}


	//Update card number
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 10;
	memcpy(szCmd, "00D695000A", iLen);

	m_pcscOp.Bin2Hex(szCmd + iLen, (unsigned char*)strTemp.GetBuffer(), 10);
	iLen += 20;

	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("更新卡号失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	//select into DF01
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand((unsigned char*)"00A4000002DF01", 14, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("选择DF目录失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	iRet = ExternalAuth_MK(DCCK, 16);
	if (iRet)
	{
		DisplayHint("认证DF主控失败;");
		return iRet;
	}

	//update DEAK
	//DEAK
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCmd, 0, sizeof(szCmd));

	iLen = 24;
	memcpy(szCmd, "80D40000170100000F77FFEE", iLen);
	m_pcscOp.Bin2Hex(szCmd + iLen, m_szCardKeys[DEK], 16);
	iLen += 32;
	iRet = ICC_SendCommand(szCmd, iLen, szRetSW, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		DisplayHint("安装DEK失败;\r\nSW: %s, return: %s", szRetSW, szRetData);
		return 4;
	}

	return 0;
}

