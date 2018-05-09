/*****************************************************************************
**
**			This file contain excutable application's comman functions
**			
**							By Jason 7/5/2018
******************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "InitialCard_HZDlg.h"


//key can be hex or bin, data should be in hex
int CInitialCard_HZDlg::TripleEncrypt(unsigned char* pszKey, int iKeyLen, unsigned char* pszData, int iDataLen, unsigned char* pszRetData)
{
	unsigned char *pData;
	unsigned char *pEncryptData;

	if (iDataLen % 16)
	{
		DisplayHint("待加密数据长度错误: %d", iDataLen);
		return 1;
	}

	pData = new unsigned char[iDataLen / 2 + 1];
	pEncryptData = new unsigned char[iDataLen + 1];
	memset(pData, 0, sizeof(pData));
	m_pcscOp.Hex2Bin(pData, pszData, iDataLen);
	if (iKeyLen == 32)
	{
		unsigned char szKey[32];
		int iLen = iKeyLen;

		memset(szKey, 0, sizeof(szKey));
		m_pcscOp.Hex2Bin(szKey, pszKey, iLen);

		des.TriDesEncrypt(szKey, iLen, pData, iDataLen / 2, pEncryptData);
	}
	else
	{
		des.TriDesEncrypt(pszKey, iKeyLen, pData, iDataLen / 2, pEncryptData);
	}

	m_pcscOp.Bin2Hex(pszRetData, pEncryptData, iDataLen / 2);

	delete[] pData;
	delete[] pEncryptData;
	pData = NULL;
	pEncryptData = NULL;
	return 0;
}

int CInitialCard_HZDlg::TripleDecrypt(unsigned char* pszKey, int iKeyLen, unsigned char* pszData, int iDataLen, unsigned char* pszRetData)
{
	unsigned char *pData;
	unsigned char *pEncryptData;

	if (iDataLen % 16)
	{
		DisplayHint("待解密数据长度错误: %d", iDataLen);
		return 1;
	}

	pData = new unsigned char[iDataLen / 2 + 1];
	pEncryptData = new unsigned char[iDataLen + 1];
	memset(pData, 0, sizeof(pData));
	m_pcscOp.Hex2Bin(pData, pszData, iDataLen);
	if (iKeyLen == 32)
	{
		unsigned char szKey[17];

		memset(szKey, 0, sizeof(szKey));
		m_pcscOp.Hex2Bin(szKey, pszKey, iKeyLen);

		des.TriDesDecrypt(szKey, iKeyLen / 2, pData, iDataLen / 2, pEncryptData);
	}
	else
	{
		des.TriDesDecrypt(pszKey, iKeyLen, pData, iDataLen / 2, pEncryptData);
	}

	m_pcscOp.Bin2Hex(pszRetData, pEncryptData, iDataLen / 2);

	delete[] pData;
	delete[] pEncryptData;
	pData = NULL;
	pEncryptData = NULL;
	return 0;
}

int CInitialCard_HZDlg::DisplayHint(char* pszFormat, ...)
{
	va_list pArgList;
	char szTemp[1024];

	memset(szTemp, 0, sizeof(szTemp));
	va_start(pArgList, pszFormat);
	vsprintf_s(szTemp, pszFormat, pArgList);
	va_end(pArgList);

	DisplayHint((CString)szTemp);

	return 0;
}

int CInitialCard_HZDlg::ClearDisplay(void)
{
	m_StrDisp.Empty();
	m_EdtDisp.SetWindowTextA("");
	m_EdtDisp.UpdateWindow();
	return 0;
}

unsigned long CInitialCard_HZDlg::RotateUID(const unsigned char* pszUID)
{
	unsigned char szTempUID[20];
	unsigned long lUID;
	int iLen;


	memset(szTempUID, 0, sizeof(szTempUID));
	if (pszUID == NULL)
		memcpy(szTempUID, "03F9D61F", 8);
	else
	{
		int j = 0;
		for (int i = 6; i != -2; i -= 2)
		{
			memcpy(szTempUID + j, pszUID + i, 2);
			j += 2;
		}
	}

	iLen = 8;
	m_eyeOp.SwitchDataMode(szTempUID, &iLen, m_eyeOp.HEX2ASC);

	lUID = szTempUID[0] * 256 * 256 * 256 + szTempUID[1] * 256 * 256 + szTempUID[2] * 256 + szTempUID[3];

	//szTempUID[0] = lUID%256;
	//szTempUID[1] = (lUID/256)%256;
	//szTempUID[2] = (lUID/256/256)%256;
	//szTempUID[3] = (lUID/256/256/256)%256;

	return lUID;
}

int CInitialCard_HZDlg::IncrementSN(void)
{
	char szSnBegin[10];
	int iLen = 0;

	m_EdtSnBegin.GetWindowTextA(m_strSnBegin);

	memset(szSnBegin, 0, sizeof(szSnBegin));

	memcpy(szSnBegin, m_strSnBegin.Right(6), 6);
	iLen = m_strSnBegin.GetLength();

	lBegin = atol(szSnBegin);

	++lBegin;
	//_ltoa_s(lBegin, szSnBegin, sizeof(szSnBegin), 10);
	sprintf_s(szSnBegin, sizeof(szSnBegin), "%06d", lBegin);
	for (int i = 0; i != 6; ++i)
	{
		m_strSnBegin.SetAt(iLen - 6 + i, szSnBegin[i]);
	}
	//增加并显示
	m_EdtSnBegin.SetWindowTextA(m_strSnBegin);


	return 0;
}

int CInitialCard_HZDlg::GetLogDirectory(char* pszPath)
{
	char szHome[200], szDriver[10], szPath[100], szFile[60], szExt[20];
	time_t t0 = time(0);
	tm tLocalTime;

	memset(szHome, 0, 200);
	memset(szDriver, 0, 10);
	memset(szPath, 0, 100);
	memset(szFile, 0, 60);
	memset(szExt, 0, 20);
	memset(m_szDate, 0, sizeof(m_szDate));

	localtime_s(&tLocalTime, &t0);
	strftime(m_szDate, sizeof(m_szDate), "%Y-%m-%d", &tLocalTime);

	::GetModuleFileNameA(AfxGetInstanceHandle(), (LPSTR)szHome, 200);
	_splitpath_s(szHome, szDriver, szPath, szFile, szExt);

	//sprintf_s(pszPath, sizeof(szPath), "%s%s\\UID_%s.txt", szDriver, szPath, m_szDate);
	sprintf_s(pszPath, sizeof(szPath), "%s%s\\UID_Data%s.mdb", szDriver, szPath, m_szDate);

	return 0;
}

////////////////////////////////////////////////////////////////////
////////////////
////////////////		related with reader
//////////////////
//////////////////////////////////
void CInitialCard_HZDlg::BeepForSuccess(void)
{
	if (m_strDeviceType.find("W8") != string::npos)
		m_eyeOp.Dev_Beep(1, 0, 3);
	else if (m_strDeviceName.find("ACS ACR128") != string::npos)
	{
	}
}

void CInitialCard_HZDlg::BeepForFail(void)
{
	m_eyeOp.Dev_Beep(2, 1, 2);
}

int CInitialCard_HZDlg::SelectCard(void)
{
	int iRet = 0, iCardProperty = 0, iCardType = 0;

	if (!m_strDeviceType.compare("W8"))
	{
		unsigned char szCardUID[10];

		memset(szCardUID, 0, sizeof(szCardUID));

		iRet = m_eyeOp.Dev_MifareRequest(0x00, &iCardType);
		if (iRet)
			return 9;

		iRet = m_eyeOp.Dev_MifareAnticoll(szCardUID);
		if (iRet)
			return 9;

		iRet = m_eyeOp.Dev_MifareSelect(szCardUID, &iCardProperty);
		if (iRet)
			return 9;

		memcpy(m_szCardUID, szCardUID, 8);
		sprintf_s((char*)m_szCardProperty, 5, "%02X", iCardProperty);
		m_szCardProperty[3] = 0;//SAK
		m_bIsSelectedCard = 1;
	}
	else
	{
		iRet = m_pcscOp.GetCardState(m_iDevIndex);
		if (iRet != 0)
			return -1;
		if (m_pcscOp.m_chStatus == m_pcscOp.CARD_PRESENT)
		{
			m_bIsSelectedCard = 1;
			return 0;//不同的卡片
		}
		else
			return 9;
	}

	return 0;
}

// 认证各目录主控密钥
int CInitialCard_HZDlg::ExternalAuth_MK(void)
{
	int iRet = 0;
	unsigned char szRetST[5], szRetData[128];
	unsigned char szRandom[32], szCommand[64];

	memset(szRetST, 0, sizeof(szRetST));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCommand, 0, sizeof(szCommand));

	//Get challenge
	iRet = ICC_SendCommand((unsigned char*)"0084000004", 10, szRetST, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		strTemp.Format("取随机数失败;\r\nSW: %s, return: %s", szRetST, szRetData);
		DisplayHint(strTemp);
		return 4;
	}

	//Form Random
	memset(szRandom, 0, sizeof(szRandom));
	memcpy(szRandom, szRetData, 8);
	memcpy(szRandom + 8, "00000000", 8);

	iRet = TripleEncrypt(m_szDCCK, 16, szRandom, 16, szRetData);
	if (iRet)
		return iRet;

	//authentication
	memset(szCommand, 0, sizeof(szCommand));
	memcpy(szCommand, "0082000008", 10);
	memcpy(szCommand + 10, szRetData, 16);

	iRet = ICC_SendCommand(szCommand, 26, szRetST, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		strTemp.Format("认证主控密钥失败;\r\nSW: %s, return: %s", szRetST, szRetData);
		DisplayHint(strTemp);
		return 4;
	}

	return 0;
}

int CInitialCard_HZDlg::ExternalAuth_MK(unsigned char* pszKey, int iKeyLen)
{
	int iRet = 0;
	unsigned char szRetST[5], szRetData[128];
	unsigned char szRandom[32], szCommand[64];

	memset(szRetST, 0, sizeof(szRetST));
	memset(szRetData, 0, sizeof(szRetData));
	memset(szCommand, 0, sizeof(szCommand));


	//Get challenge
	iRet = ICC_SendCommand((unsigned char*)"0084000008", 10, szRetST, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		strTemp.Format("取随机数失败;\r\nSW: %s, return: %s", szRetST, szRetData);
		DisplayHint(strTemp);
		return 4;
	}

	//Form Random
	memset(szRandom, 0, sizeof(szRandom));
	memcpy(szRandom, szRetData, 16);

	memset(szRetData, 0, sizeof(szRetData));
	iRet = TripleEncrypt(pszKey, iKeyLen, szRandom, 16, szRetData);
	if (iRet)
		return iRet;

	//authentication
	memset(szCommand, 0, sizeof(szCommand));
	memcpy(szCommand, "0082000008", 10);
	memcpy(szCommand + 10, szRetData, 16);

	memset(szRetST, 0, sizeof(szRetST));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = ICC_SendCommand(szCommand, 26, szRetST, szRetData);
	if (iRet < 0 || iRet > 255)
	{
		strTemp.Format("认证主控密钥失败;\r\nSW: %s, return: %s", szRetST, szRetData);
		DisplayHint(strTemp);
		return 4;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////
//复位卡片，寻卡的功能
////////////////////////////////////////////////////////////////////////////
int CInitialCard_HZDlg::UserCardReset(void)
{
	int iRet = 0;
	unsigned char szRetSW[5], szRetData[128];
	memset(szRetSW, 0, sizeof(szRetSW));
	memset(szRetData, 0, sizeof(szRetData));
	memset(m_szATS, 0, sizeof(m_szATS));
	memset(m_szATR, 0, sizeof(m_szATR));

	if (!m_strDeviceType.compare("W8"))
	{
		if (m_eyeOp.Dev_CardReset(m_eyeOp.USERCARD, szRetData, m_szATS) != 0)
			return 1;//未复位成功

		memcpy(m_szATR, m_szATS, strlen((char*)m_szATS));
	}
	else
	{
		int iRet = m_pcscOp.PowerOnCard(m_iDevIndex);
		if (iRet)//power on fail
			return 9;

		memcpy(m_szATR, m_pcscOp.m_szAtr, m_pcscOp.m_iAtrLen);
		memcpy(m_szATS, m_pcscOp.m_szAtr, m_pcscOp.m_iAtrLen);
	}

	if (m_bIsSelectedCard == 1)
	{
		memset(m_szDIV, 0, sizeof(m_szDIV));
		if (!strCSNType.compare("APDU"))
		{
			iRet = ICC_SendCommand((BYTE*)strCSNCommand.c_str(), strCSNCommand.length(), szRetSW, szRetData);
			if (iRet != 0)
			{
				/***从ATS中得到,偏移地址为从24开始16字符, 仅适于未发卡的P5CD012-9***/
				if (!m_strDeviceType.compare("W8"))
				{
					memcpy(m_szDIV, m_szATR + 24, 8);
					memcpy(m_szDIV + 8, "12112910", 8);
					memcpy(m_szCardUID, m_szDIV, 8);
				}
				else
				{
					memcpy(m_szDIV, m_szATR + 22, 16);
					memcpy(m_szCardUID, m_szDIV, 8);
				}
			}
			else
			{
				memcpy(m_szDIV, szRetData + m_iUID_Offset * 2, 16);
				memcpy(m_szCardUID, m_szDIV, 8);
			}
		}
		else if (!strCSNType.compare("ATS"))
		{
			if (!m_strDeviceType.compare("PCSC"))
			{
				if (m_strDeviceName.find("SDI") != string::npos)
				{
					memset(m_szATS, 0, sizeof(m_szATS));
					iRet = ICC_SendCommand((BYTE*)"FFCC00000193", 12, szRetSW, szRetData);
					if (iRet< 0)
					{
						DisplayHint("取ATS失败!");
						return 9;
					}
					memcpy(m_szDIV, szRetData + m_iUID_Offset * 2, 16);
					memcpy(m_szCardUID, m_szDIV, 8);
				}
				else if (m_strDeviceName.find("ACS ACR1281 1S Dual Reader PICC") != string::npos)
				{
					memset(m_szATS, 0, sizeof(m_szATS));
					iRet = ICC_SendCommand((BYTE*)"FFCA010000", 10, szRetSW, szRetData);
					if (iRet< 0)
					{
						DisplayHint("取ATS失败!");
						return 9;
					}
					memcpy(m_szDIV, szRetData + m_iUID_Offset * 2, 16);
					memcpy(m_szCardUID, m_szDIV, 8);
				}
			}

			else
			{
				memcpy(m_szDIV, m_szATS + m_iUID_Offset * 2, 16);
				memcpy(m_szCardUID, m_szDIV, 8);

			}

		}
		else if (!strCSNType.compare("ATR"))
		{
			if (!m_strDeviceType.compare("PCSC"))
			{
				memcpy(m_szATR, m_pcscOp.m_szAtr, m_pcscOp.m_iAtrLen);
				memcpy(m_szATS, m_pcscOp.m_szAtr, m_pcscOp.m_iAtrLen);
				memcpy(m_szDIV, m_szATS + m_iUID_Offset * 2, 16);
				memcpy(m_szCardUID, m_szDIV, 8);
			}
		}
		else if (!strCSNType.compare("CARD_NO"))
		{
			strTemp.Empty();
			m_EdtSnBegin.GetWindowTextA(strTemp);

			m_pcscOp.Bin2Hex(m_szDIV, (unsigned char*)strTemp.Right(8).GetBuffer(), 8);

		}
		else
		{
			DisplayHint("CSN类型设置错误：	%s", strCSNType.c_str());
			return 9;
		}


		strTemp.Empty();
		m_EdtSnBegin.GetWindowTextA(strTemp);

		//SN from frame
		memset(m_szSN, 0, sizeof(m_szSN));
		memcpy(m_szSN, strTemp.GetBuffer(), strTemp.GetLength());
		m_bIsSelectedCard = 0;
	}
	return 0;
}


int CInitialCard_HZDlg::CheckCommand(string strCommand)
{
	string strTemp;
	int iRet = 0, iOffset = 0, iLength = 0, iOffsetR = 0, iLen;
	unsigned char szTempData[512];

	if (/*strCommand.find(0x5B) != string::npos*/strCommand.at(0) == 0x5B)// '['
	{
		int iSrcLen = 0;
		if ((iOffsetR = strCommand.find("%REPLACE%")) != string::npos)
		{//replace data
			strTemp.clear();
			strTemp.append(strCommand.c_str(), iOffsetR - iOffset);
			strTemp.append(m_strReplace);
			iOffset = strCommand.find(0x25, iOffsetR + 1) + 1;
			iOffsetR = strCommand.length();

			strTemp.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);
			strCommand.clear();
			strCommand.append(strTemp);
		}

		if (!strCommand.compare(0, 7, "[RESET]", 7))
		{//reset
			if (!m_strDeviceType.compare("PCSC"))
			{
				iRet = m_pcscOp.ResetCard(m_iDevIndex);
				if (iRet)
				{
					DisplayHint("卡片复位失败!");
					return 1;
				}
			}
			else
				return UserCardReset();
		}
		else if (strCommand.find("EX_AUTH") != string::npos)
		{// external authentication
			if ((iRet = strCommand.find("DCCK")) != string::npos)
			{
				return ExternalAuth_MK();
			}
			else
			{
				unsigned char szKey[33];
				int iOffset = 0;

				memset(szKey, 0, sizeof(szKey));

				iOffset = strCommand.find(0x25);
				memcpy(szKey, strCommand.c_str() + iOffset + 1, 32);

				return ExternalAuth_MK(szKey, 32);
			}
		}
		else if (strCommand.find("SAVE") != string::npos)
		{//保存预置数据
			int iOffset = 0;
			int iLength = 0;

			m_strReplace.clear();

			iOffset = strCommand.find(0x5D);
			iLength = iOffset + 1;

			if (strCommand.find(0x25) != string::npos)
			{
				m_strReplace = strCommand.substr(iLength, (iOffset = strCommand.find(0x25)) - iLength);

				if (strCommand.find("UID") != string::npos)
				{
					m_strReplace.append((char*)m_szCardUID, 8);
				}
				else if (strCommand.find("CSN") != string::npos)
				{
					m_strReplace.append((char*)m_szDIV, 16);
				}

				iLength = iOffset + 1;
				if ((iOffset = strCommand.find(0x25, iLength) + 1) != (iLength = strCommand.length()))
				{// copy last part of data
					iLength = iLength - (iOffset + 1);
					m_strReplace.append(strCommand.c_str() + iOffset + 1, iLength);
				}
			}
			else//no '%', Kind of command
			{
				unsigned char szRetSW[5], szRetData[520];

				memset(szRetSW, 0, sizeof(szRetSW));
				memset(szRetData, 0, sizeof(szRetData));
				strTemp.clear();
				strTemp = strCommand.substr(iLength, strCommand.length());
				iRet = ICC_SendCommand((unsigned char*)strTemp.c_str(), strTemp.length(), szRetSW, szRetData);
				if (iRet)
					return iRet;

				m_strReplace.clear();
				m_strReplace.append((char*)szRetData);
			}

			return 0;

		}
		else if (strCommand.find("ENCRYPT") != string::npos)
		{
			iOffset = strCommand.find(0x5D) + 1;
			iLength = strCommand.length() - iOffset;
			strTemp.clear();
			strTemp.append(strCommand.c_str() + iOffset, strCommand.length() - iOffset);
			if ((strTemp.length() % 16) != 0)
			{
				DisplayHint("待加密的数据<%s>长度错误;", strTemp.c_str());
				return 1;
			}

			string strKey;
			memset(szTempData, 0, sizeof(szTempData));
			strKey.clear();
			if (strCommand.find("DCCK") != string::npos)
			{
				strKey.append((char*)m_szDCCK, 16);
			}
			else if (strCommand.find("DTMK") != string::npos)
			{
				strKey.append((char*)m_szDTMK, 16);
			}
			else if (strCommand.find("DCAK") != string::npos)
			{
				strKey.append((char*)m_szDCAK, 16);
			}
			else
			{
				int iOffsetR = 0;

				iOffset = strCommand.find(0x25) + 1;
				iOffsetR = strCommand.find(0x25, iOffset);

				strKey.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);
			}

			iRet = TripleEncrypt((unsigned char*)strKey.c_str(), strKey.length(), (unsigned char*)strTemp.c_str(), iLength, szTempData);
			if (iRet)
				return iRet;

			m_strReplace.clear();
			m_strReplace.append((char*)szTempData, strTemp.length());

			return 0;
		}
		else if (strCommand.find("DECRYPT") != string::npos)
		{
			int iOffsetR = 0;
			string strKey;

			iOffset = strCommand.find(0x5D) + 1;
			iLength = strCommand.length() - iOffset;
			strTemp.clear();
			strTemp.append(strCommand.c_str() + iOffset, strCommand.length() - iOffset);


			memset(szTempData, 0, sizeof(szTempData));

			if (strCommand.find("DCCK") != string::npos)
			{
				strKey.append((char*)m_szDCCK, 16);
			}
			else if (strCommand.find("DTMK") != string::npos)
			{
				strKey.append((char*)m_szDTMK, 16);
			}
			else if (strCommand.find("DCAK") != string::npos)
			{
				strKey.append((char*)m_szDCAK, 16);
			}
			else
			{
				iOffsetR = 0;
				iOffset = strCommand.find(0x25) + 1;
				iOffsetR = strCommand.find(0x25, iOffset);

				strKey.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);
			}
			iRet = TripleDecrypt((unsigned char*)strKey.c_str(), strKey.length(), (unsigned char*)strTemp.c_str(), iLength, szTempData);
			if (iRet)
				return iRet;

			m_strReplace.clear();
			m_strReplace.append((char*)szTempData, strTemp.length());

			return 0;
		}
		else
		{
			DisplayHint("无法识别的指令 >> %s", strCommand.c_str());
			return 1;
		}

	}
	else if (strCommand.find(0x28) != string::npos)// '('
	{
		int iSrcLen = 0;
		if ((iOffsetR = strCommand.find("%REPLACE%")) != string::npos)
		{
			iOffset = strCommand.find(0x29) + 1;
			strTemp.clear();
			strTemp.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);
			strTemp.append(m_strReplace);
			iOffset = strCommand.find(0x25, iOffsetR + 1) + 1;
			iOffsetR = strCommand.length();

			strTemp.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);
		}
		else
		{//析取宏后的指令数据
			iOffset = strCommand.find(0x29) + 1;
			iOffsetR = strCommand.length();

			strTemp.clear();
			strTemp.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);
		}

		m_pcscOp.Hex2Bin(szTempData, (unsigned char*)strTemp.c_str(), strTemp.length());
		iSrcLen = strTemp.length() / 2;

		if (strCommand.find("CMAC") != string::npos)
		{//cal mac
			unsigned char szMAC[10];
			unsigned char szMacKey[33];
			memset(szMacKey, 0, sizeof(szMacKey));

			if (strCommand.find("DCAK") != string::npos)
			{
				memcpy(szMacKey, m_szDCAK, 16);
				szMacKey[16] = 0;
			}
			else if (strCommand.find("DTMK") != string::npos)
			{
				memcpy(szMacKey, m_szDTMK, 16);
				szMacKey[16] = 0;
			}
			else if (strCommand.find("DCMK") != string::npos)
			{
				memcpy(szMacKey, m_szDCMK, 16);
				szMacKey[16] = 0;
			}
			else if (strCommand.find("DCCK") != string::npos)
			{
				memcpy(szMacKey, m_szDCCK, 16);
				szMacKey[16] = 0;
			}
			else
			{
				iOffset = strCommand.find(0x25) + 1;
				memcpy(szMacKey, strCommand.c_str() + iOffset, 32);
				szMacKey[32] = 0;

				iLen = 32;
				m_pcscOp.SwitchDataMode(szMacKey, &iLen, m_pcscOp.HEX2BIN);
				szMacKey[16] = 0;
			}

			unsigned char szRetSW[5], szRetData[128];
			memset(szRetSW, 0, sizeof(szRetSW));
			memset(szRetData, 0, sizeof(szRetData));

			iRet = ICC_SendCommand((BYTE*)"0084000004", 10, szRetSW, szRetData);
			if (iRet < 0 || iRet > 255)
			{
				if (!memcmp(szRetSW, "6A81", 4))
				{
					iRet = ICC_SendCommand((BYTE*)"80FE000004", 10, szRetSW, szRetData);
					if (iRet <0 || iRet > 255)
					{
						DisplayHint("取随机数指令失败, 返回:\r\nSW: %s;	-->%s", szRetSW, m_pcscOp.m_strError);
						return 1;
					}
				}
				else
				{
					DisplayHint("取随机数指令失败, 返回:\r\nSW: %s;	-->%s", szRetSW, m_pcscOp.m_strError);
					return 1;
				}

			}

			memcpy(szRetData + 8, "00000000", 8);
			iLen = 16;
			m_pcscOp.SwitchDataMode(szRetData, &iLen, m_pcscOp.HEX2BIN);


			memset(szMAC, 0, sizeof(szMAC));
			des.CalMAC(szMacKey, 16, szRetData, szTempData, iSrcLen, szMAC);
			iLen = 4;
			m_pcscOp.SwitchDataMode(szMAC, &iLen, m_pcscOp.BIN2HEX);

			strTemp.append((char*)szMAC, 8);

			memset(szRetSW, 0, sizeof(szRetSW));
			iRet = ICC_SendCommand((unsigned char*)strTemp.c_str(), strTemp.length(), szRetSW, szRetData);
			if (iRet < 0 || iRet > 255)
			{
				DisplayHint("发送指令<%s>失败, 返回:\r\nSW: %s;	-->%s", strTemp.c_str(), szRetSW, m_pcscOp.m_strError);
				return 1;
			}
			return 0;
		}
		else if (strCommand.find("SN") != string::npos)//写入序号
		{//m_strSnBegin 输入的序列号，5字符
			iOffset = strCommand.find(0x29) + 1;
			iOffsetR = strCommand.find(0x25);
			strTemp.clear();
			strTemp.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);

			char szLength[10];
			memset(szLength, 0, sizeof(szLength));

			iOffset = iOffsetR + 1;
			iOffsetR = strCommand.find(0x25, iOffset);
			memcpy(szLength, strCommand.c_str() + iOffset, iOffsetR - iOffset);
			iLen = atoi(szLength);//要写入的长度

			unsigned char szUID_LSB[20];
			int iSNLen = 0;

			memset(m_szSN, 0, sizeof(m_szSN));
			memset(szUID_LSB, 0, sizeof(szUID_LSB));

			iRet = GetSN(m_szDIV, m_szSN, iSNLen);
			if (iRet)
				return iRet;

			string strNewCmd;
			strTemp.append((char*)m_szSN, iSNLen);

			return CheckCommand(strTemp);
		}
		else if (strCommand.find("DISPLAY") != string::npos)//显示指令的结果
		{
			//iOffset = strCommand.find(0x29) + 1;
			//iOffsetR = strCommand.length();

			//strTemp.append(strCommand.c_str() + iOffset, iOffsetR - iOffset);

			////已析取纯指令
			unsigned char szRetSW[5], szRetData[520];
			memset(szRetSW, 0, sizeof(szRetSW));
			memset(szRetData, 0, sizeof(szRetData));

			iRet = ICC_SendCommand((unsigned char*)strTemp.c_str(), strTemp.length(), szRetSW, szRetData);
			if (iRet)
			{
				DisplayHint("指令<%s>执行失败；\r\nSW: %s", strTemp.c_str(), szRetSW);
				return 1;
			}
			DisplayHint("<<    %s    >>\r\n", szRetData);
			return 0;
		}
		else if (strCommand.find("CMD") != string::npos)
		{
			unsigned char szRetSW[5], szRetData[520];
			memset(szRetSW, 0, sizeof(szRetSW));
			memset(szRetData, 0, sizeof(szRetData));

			iRet = ICC_SendCommand((unsigned char*)strTemp.c_str(), strTemp.length(), szRetSW, szRetData);
			if (iRet)
			{
				DisplayHint("指令<%s>执行失败；\r\nSW: %s", strTemp.c_str(), szRetSW);
				return 1;
			}
			return 0;
		}
		else
		{
			DisplayHint("未知指令<%s>;", strCommand);
			return 1;
		}

	}
	else// send command
	{
		unsigned char szRetSW[5], szRetData[512];
		memset(szRetSW, 0, sizeof(szRetSW));
		memset(szRetData, 0, sizeof(szRetData));

		iRet = ICC_SendCommand((unsigned char*)strCommand.c_str(), strCommand.length(), szRetSW, szRetData);
		if (iRet < 0 || iRet > 255)
		{
			DisplayHint("发送指令<%s>失败, 返回:\r\nSW: %s;	-->%s", strCommand.c_str(), szRetSW, m_pcscOp.m_strError);
			return 1;
		}

		return 0;
	}

	return 0;
}
