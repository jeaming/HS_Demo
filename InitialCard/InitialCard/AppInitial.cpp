/*****************************************************************************
**
**			This file initialize current excutable application
**			It will set varables as expected
**
**							By Jason 5/5/2018
******************************************************************************/

#include "stdafx.h"
#include "resource.h"
#include "InitialCard_HZDlg.h"

#define SETTING_FILE "PersoSet.xml"
#define CONTROL_KEY		(unsigned char*)"\x0C\x57\x5A\x62\x1A\x78\xCB\xEE\x84\x66\x32\x7A\x11\xEA\xB3\x15"

#define	PORT "USB1"
#define BAUD 2
#define CHECKSUM	0x69

#define TREATHR(hr)	if FAILED(hr) _com_issue_error(hr);

int CInitialCard_HZDlg::CreateAccessFile(char* pszFilePath)
{
	HRESULT hr = S_OK;
	ADOX::_CatalogPtr m_pCatalog;
	CString strConn(_T("Provider=Microsoft.JET.OLEDB.4.0;Data source="));
	strConn += pszFilePath;

	try
	{
		m_pCatalog = NULL;
		TREATHR(hr = m_pCatalog.CreateInstance(__uuidof(ADOX::Catalog)));
		//创建新数据库
		m_pCatalog->Create(_bstr_t(strConn));
		m_isNewDB = 1;

	}
	catch (_com_error &err)
	{
		strConn.Format("创建数据库失败!\n\tSource:	%s\n\tDescription:	%s\n\tMessage:	%s", (LPCTSTR)(_bstr_t)err.Source(), (LPCTSTR)(_bstr_t)err.Description(), err.ErrorMessage());
		AfxMessageBox(strConn);
		return 1;
	}

	return 0;
}

//	Initialize database
int CInitialCard_HZDlg::InitDatabase()
{
	int iRet = 0;

	memset(m_szFilePath, 0, sizeof(m_szFilePath));
	GetLogDirectory(m_szFilePath);//得到路径

	if (m_szFilePath[0] == 0)
	{
		AfxMessageBox("获取日志文件目录失败，继续发卡将无法记录卡片UID信息!");
		exit(-1);
	}

	m_strConn.Format(_T("Provider=Microsoft.JET.OLEDB.4.0;Data source="));
	m_strConn += m_szFilePath;

	CFileFind ff;
	AfxOleInit();
	if (ff.FindFile(m_szFilePath) == FALSE)
	{
		iRet = CreateAccessFile(m_szFilePath);
		if (iRet)
			exit(1);
	}

	////////////////////////////////////////////////////////
	//////////////////连接数据库
	HRESULT hr;
	try
	{
		hr = m_pConn.CreateInstance("ADODB.Connection");//create object connection
		if (SUCCEEDED(hr))
		{
			hr = m_pConn->Open(_bstr_t(m_strConn), "", "", 0);//adModeUnknown
			if (FAILED(hr))
			{
				_com_issue_error(hr);
			}
		}
	}
	catch (_com_error &err)
	{
		CString strErr;
		strErr.Format("数据库连接失败!\r\n%s", err.ErrorMessage());
		AfxMessageBox(strErr);
		exit(1);
	}

	m_varNull.vt = VT_ERROR;
	m_varNull.scode = DISP_E_PARAMNOTFOUND;
	////////////////////////////////////////////////////
	///////////////////初始化SQL指令执行
	try
	{
		hr = m_pSqlCommand.CreateInstance("ADODB.Command");
		if (SUCCEEDED(hr))
		{
			m_pSqlCommand->ActiveConnection = m_pConn;
			m_pSqlCommand->CommandTimeout = 5;

			if (m_isNewDB)
			{//database is created without created table, create it;
				m_pSqlCommand->CommandText = _bstr_t("CREATE TABLE UidLog(SN text(20) NOT NULL, UID text(20) NOT NULL, CardType text(5),FACTRY text(10) NOT NULL,INITIAL_DATE text(20) NOT NULL,ROTATE_UID text(20) NOT NULL,CSN text(20) NOT NULL,ATS text(96),MIFARE_INITED text(10),PATCH text(8));");
				m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

				m_pSqlCommand->CommandText = _bstr_t("CREATE INDEX indexSN ON UidLog (SN);");
				m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

				m_pSqlCommand->CommandText = _bstr_t("CREATE INDEX indexUID ON UidLog (UID,SN);");
				m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

				//database is created without created table, create it;
				m_pSqlCommand->CommandText = _bstr_t("CREATE TABLE SN_DATA(SN text(20) NOT NULL, UID_LSB text(20) NOT NULL,ROTATE_UID text(20),PERSO_STATE text(20));");
				m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

				m_pSqlCommand->CommandText = _bstr_t("CREATE INDEX indexSN ON SN_DATA (SN);");
				m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

				m_pSqlCommand->CommandText = _bstr_t("CREATE INDEX indexUID ON SN_DATA (UID_LSB,SN);");
				m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

				m_isNewDB = 0;
			}

		}
		else
			_com_issue_error(hr);
	}
	catch (_com_error &err)
	{
		CString strErr;
		strErr.Format("数据库指令初始化失败!\r\n%s", err.ErrorMessage());
		AfxMessageBox(strErr);
		exit(1);
	}

	return 0;
}

int CInitialCard_HZDlg::LogUID(void)
{
	m_pSqlCommand->CommandText = _bstr_t(m_strUIDLog.GetBuffer());

	m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

	//CString strTemp;
	//strTemp.Format("UPDATE SN_DATA SET PERSO_STATE = '1' WHERE SN = '%s';", m_szSN);
	//m_pSqlCommand->CommandText = _bstr_t(strTemp.GetBuffer());
	//m_pSqlCommand->Execute(&m_varNull, &m_varNull, adCmdText);

	return 0;
}

//设置对话框中的显示字体大小
int CInitialCard_HZDlg::SetWindFont()
{

	fontA.CreateFontA(20, 12, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "楷体");

	m_brush.CreateSolidBrush(RGB(255, 255, 0));

	m_EdtSnBegin.SetFont(&fontA);

	fontB.CreateFontA(20, 14, 0, 0, 700, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "楷体");

	m_EdtDisp.SetFont(&fontB);

	return 0;
}

//设置发卡类型下拉框
void CInitialCard_HZDlg::SetPersoTypeCombo()
{
	m_cmbPersoType.InsertString(ADMIN_CARD, "管理卡");
	m_cmbPersoType.InsertString(SINGLE_CAR_CARD, "单车卡");
	m_cmbPersoType.InsertString(AGENCY_CARD, "单位卡");
	m_cmbPersoType.InsertString(PSAM_CARD, "PSAM卡");
	m_cmbPersoType.SetCurSel(ADMIN_CARD);
}

// Get current device set
int CInitialCard_HZDlg::GetInitSettings(void)
{
	using namespace std;
	using namespace boost::property_tree;
	int iRet = 0, iLen = 0;
	string strData, strCAK, strTMK, strDCCK;
	unsigned char szKey[40];
	char szPath[MAX_PATH];

	ptree xmlTree;

	try
	{
		memset(m_szModulePath, 0, sizeof(m_szModulePath));

		iRet = GetModulePath(m_szModulePath);
		memset(szPath, 0, sizeof(szPath));
		sprintf_s(szPath, sizeof(szPath), "%s%s", m_szModulePath, SETTING_FILE);
		read_xml(szPath, xmlTree);

		m_strCity = xmlTree.get<string>("SETTINGS.<xmlattr>.CITY");
		strPatch = xmlTree.get<string>("SETTINGS.PATCH");
		strInterface = xmlTree.get<string>("SETTINGS.HARDWARE.INTERFACE");

		strPersoType = xmlTree.get<string>("SETTINGS.<xmlattr>.PERSO_TYPE");
		if (strPersoType.find("GET_UID") != string::npos)
			m_BtnStart.SetWindowTextA("只读UID");

		//if pcsc: user device name as connect string
		m_strDeviceType = xmlTree.get<string>("SETTINGS.HARDWARE.DeviceType");
		m_strDeviceName = xmlTree.get<string>("SETTINGS.HARDWARE.DeviceName");

		//Set Dialog title
		strData = "";
		strData += xmlTree.get<string>("SETTINGS.<xmlattr>.REALM");
		strData += " - ";
		strData += m_strDeviceType;
		this->SetWindowTextA((LPCTSTR)strData.c_str());

		//get CSN type
		strCSNType = xmlTree.get<string>("SETTINGS.SECURITY.CSN.<xmlattr>.type");
		if (!strCSNType.compare("APDU"))
			strCSNCommand = xmlTree.get<string>("SETTINGS.SECURITY.CSN.APDU");
		else
			strCSNCommand.clear();

		m_iUID_Offset = xmlTree.get<int>("SETTINGS.SECURITY.CSN.UID_OFFSET");
		strMifareInit = xmlTree.get<string>("SETTINGS.FUNCTIONAL.MifareAccInit");

		m_iOutSend = xmlTree.get<int>("SETTINGS.FUNCTIONAL.OUT_FIT");
		m_iCategory = xmlTree.get<int>("SETTINGS.FUNCTIONAL.CATEGORY");
		if (m_iCategory != 99)
		{
			SetPersoAPDU(m_iCategory);
			m_cmbPersoType.SetCurSel(m_iCategory);
			m_cmbPersoType.EnableWindow(FALSE);

			if (m_iCategory == 3)
				strMifareInit = "DISABLE";
		}

		strCAK = xmlTree.get<string>("SETTINGS.SECURITY.CAK");
		strTMK = xmlTree.get<string>("SETTINGS.SECURITY.TMK");
		strDCCK = xmlTree.get<string>("SETTINGS.SECURITY.DCCK");

		//APDU
		strAPDU = xmlTree.get<string>("SETTINGS.DATA_USER_CARD");
		if ((iRet = DecryptData(strAPDU)) != 0)
		{
			AfxMessageBox((CString)strAPDU.c_str());
			AfxMessageBox("解析发卡指令失败!");
			return 1;
		}

		strChkConstructAPDU = xmlTree.get<string>("SETTINGS.CHECK_CONSTRUCT_USER_CARD");
		strRecoveryAPDU = xmlTree.get<string>("SETTINGS.RECOVERY_USER_CARD");
	}
	catch (std::exception &err)
	{
		string strErr;
		strErr = err.what();
		AfxMessageBox(strErr.c_str());
		return 1;
	}

	memset(m_szDCCK, 0, sizeof(m_szDCCK));
	memset(m_szCAK, 0, sizeof(m_szCAK));
	memset(m_szTMK, 0, sizeof(m_szTMK));

	memset(szKey, 0, sizeof(szKey));
	//CAK
	m_pcscOp.Hex2Bin(szKey, (unsigned char*)strCAK.c_str(), strCAK.length());
	des.TriDesDecrypt(CONTROL_KEY, 16, (unsigned char*)szKey, 16, m_szCAK);

	//TMK
	m_pcscOp.Hex2Bin(szKey, (unsigned char*)strTMK.c_str(), strTMK.length());
	des.TriDesDecrypt(CONTROL_KEY, 16, (unsigned char*)szKey, 16, m_szTMK);

	//CAK
	m_pcscOp.Hex2Bin(szKey, (unsigned char*)strDCCK.c_str(), strDCCK.length());
	des.TriDesDecrypt(CONTROL_KEY, 16, (unsigned char*)szKey, 16, m_szDCCK);

	//GetDialogTitle();

	return 0;
}

//更改发卡指令流
int CInitialCard_HZDlg::SetPersoAPDU(int iType)
{
	using namespace std;
	using namespace boost::property_tree;
	int iRet = 0, iLen = 0;
	string strData;
	char szPath[MAX_PATH];

	if (m_isReaderOpen)
	{
		if (!m_strDeviceType.compare("W8"))
			m_eyeOp.Dev_DisconnReader();

		//reader close
		m_isReaderOpen = FALSE;
	}


	ptree xmlTree;

	try
	{
		memset(m_szModulePath, 0, sizeof(m_szModulePath));

		iRet = GetModulePath(m_szModulePath);
		memset(szPath, 0, sizeof(szPath));
		sprintf_s(szPath, sizeof(szPath), "%s\\%s", m_szModulePath, SETTING_FILE);
		read_xml(szPath, xmlTree);

		//Set Dialog title
		strData = "";
		m_strDeviceType = xmlTree.get<string>("SETTINGS.HARDWARE.DeviceType");
		m_strDeviceName = xmlTree.get<string>("SETTINGS.HARDWARE.DeviceName");//Get device type,if it is PSAM perso,device type will change to PCSC type.

		if (iType == 0)
			strData += xmlTree.get<string>("SETTINGS.<xmlattr>.REALM_ADMIN");
		else if (iType == 1)
			strData += xmlTree.get<string>("SETTINGS.<xmlattr>.REALM_SINGLE_CAR");
		else if (iType == 2)
			strData += xmlTree.get<string>("SETTINGS.<xmlattr>.REALM_AGENCY");
		else
			strData += xmlTree.get<string>("SETTINGS.<xmlattr>.REALM_PSAM");


		strData += " - ";
		strData += m_strDeviceType;
		this->SetWindowTextA((LPCTSTR)strData.c_str());

		//get perso apdu flows
		if (iType == 3)
		{
			strAPDU = xmlTree.get<string>("SETTINGS.DATA_PSAM");
			m_strDeviceName = xmlTree.get<string>("SETTINGS.HARDWARE.DeviceNameContact");//Get device type,if it is PSAM perso,device type will change to PCSC type.
			m_strDeviceType = "PCSC";
			strInterface = "CONTACT";

			strRecoveryAPDU = xmlTree.get<string>("SETTINGS.RECOVERY_PSAM");
			strChkConstructAPDU = xmlTree.get<string>("SETTINGS.CHECK_CONSTRUCT_PSAM");
		}
		else
		{
			strAPDU = xmlTree.get<string>("SETTINGS.DATA_USER_CARD");
			strInterface = xmlTree.get<string>("SETTINGS.HARDWARE.INTERFACE");

			strRecoveryAPDU = xmlTree.get<string>("SETTINGS.RECOVERY_USER_CARD");
			strChkConstructAPDU = xmlTree.get<string>("SETTINGS.CHECK_CONSTRUCT_USER_CARD");
		}

		if ((iRet = DecryptData(strAPDU)) != 0)
		{
			AfxMessageBox((CString)strAPDU.c_str());
			AfxMessageBox("解析发卡指令失败!");
			return 1;
		}

		strMifareInit = "DISABLE";
	}
	catch (std::exception &err)
	{
		string strErr;
		strErr = err.what();
		AfxMessageBox(strErr.c_str());
		return 1;
	}

	return 0;
}

//连接读写器
int CInitialCard_HZDlg::ConnectReader()
{
	if (m_isReaderOpen)
		return 0;

	int iRet = 0;

	if (m_eyeOp.iErrorCode != 0 && !m_strDeviceType.compare("W8"))
	{
		strTemp.Format("动态库未成功装载(%d)，将退出程序 ...", m_eyeOp.iErrorCode);
		AfxMessageBox(strTemp);
		exit(m_eyeOp.iErrorCode);
	}

	if (!m_strDeviceType.compare("PCSC"))
	{
		m_iDevIndex = m_pcscOp.ConnectReader(m_strDeviceName.c_str());
		if (m_iDevIndex == -1)
		{
			AfxMessageBox(m_pcscOp.m_strError);
			return -1;
		}
	}
	else if (!m_strDeviceType.compare("W8"))
	{
		iRet = m_eyeOp.Dev_ConnectReader(PORT, BAUD, CHECKSUM);
		if (iRet)
		{
			AfxMessageBox("读卡器未连接或连接不正确，请检查！");
			return -1;
		}

		DisplayHint("读卡器已连接, 状态正常;");
		return 0;
	}
	else
	{
		m_strDeviceType.append(":读写器类型设置不正确;");
		AfxMessageBox(m_strDeviceType.c_str());
		//exit(-4);//不退出应用使用时检查读写器连接
	}
	return 0;
}


void CInitialCard_HZDlg::OnCbnCloseupComboPersoType()
{
	// TODO:  在此添加控件通知处理程序代码
	SetPersoAPDU(m_cmbPersoType.GetCurSel());
}


void CInitialCard_HZDlg::OnCbnKillfocusComboPersoType()
{
	// TODO:  在此添加控件通知处理程序代码
	SetPersoAPDU(m_cmbPersoType.GetCurSel());
}



//////////////////////////////////////////
int CInitialCard_HZDlg::GetModulePath(char* pszFilePath)
{
	char szHome[200], szDriver[10], szPath[100], szFile[60], szExt[20];

	memset(szHome, 0, 200);
	memset(szDriver, 0, 10);
	memset(szPath, 0, 100);
	memset(szFile, 0, 60);
	memset(szExt, 0, 20);
	::GetModuleFileNameA(AfxGetInstanceHandle(), (LPSTR)szHome, 200);
	_splitpath_s((char*)szHome, szDriver, szPath, szFile, szExt);

	sprintf_s(pszFilePath, 200, "%s%s", szDriver, szPath);
	return 0;
}

//in hex
int CInitialCard_HZDlg::RemoveDataPad(unsigned char* pszData, unsigned int* iDataLen)
{
	unsigned int iLength = *iDataLen;

	if (iLength < 16 || iLength % 2)
		return 1;

	if (!memcmp(&pszData[iLength - 16], "8000000000000000", 16))
	{
		pszData[iLength - 16] = 0;
		*iDataLen = iLength - 16;
	}
	else if (!memcmp(&pszData[iLength - 16], "0000000000000000", 16))
	{
		pszData[iLength - 18] = 0;
		*iDataLen = iLength - 18;
	}
	else
	{
		int iLen = 4;
		iDataLen -= 4;
		while (!memcmp(&pszData[iLength - iLen], "00", 2))
		{
			if (iLen > 16)
				return 1;
			*iDataLen -= 2;
			iLen += 2;
		}
		if (!memcmp(&pszData[iLength - iLen], "80", 2))
		{
			pszData[iLength - iLen] = 0;
			*iDataLen -= 2;
		}
		else
			return 1;
	}
	return 0;
}

//解析加密后的指令
int CInitialCard_HZDlg::DecryptData(string &strData)
{
	unsigned char szData[550], szTemp[550];
	unsigned int iDataLen = 0, iOffset = 0, iPos = 0, iLength = 0, iRet = 0;
	CString strTemp;

	iDataLen = strData.length();
	memset(szData, 0, sizeof(szData));
	memset(szTemp, 0, sizeof(szTemp));


	while ((iPos = strData.find(0x0A, iOffset)) != string::npos)
	{
		memcpy(szData, strData.c_str() + iOffset, (iLength = iPos - iOffset));
		szData[iLength] = 0;
		if (szData[0] == 0x5B || szData[0] == 0x28)
		{
			strTemp += (char*)szData;
			strTemp += "\n";
			iOffset = iPos + 1;
			continue;
		}

		if (iLength < 16)
			return 1;
		m_pcscOp.Hex2Bin(szTemp, szData, iLength);
		memset(szData, 0, sizeof(szData));
		des.TriDesDecrypt(CONTROL_KEY, 16, szTemp, iLength / 2, szData);

		memset(szTemp, 0, sizeof(szTemp));
		m_pcscOp.Bin2Hex(szTemp, szData, iLength / 2);
		iRet = RemoveDataPad(szTemp, &iLength);
		if (iRet)
		{
			return 1;
		}
		strTemp += (char*)szTemp;
		strTemp += "\n";
		iOffset = iPos + 1;
	}

	if (iOffset < iDataLen)
	{
		memcpy(szData, strData.c_str() + iOffset, (iLength = iDataLen - iOffset));
		szData[iLength] = 0;
		if (szData[0] != 0x5B && szData[0] != 0x28)
		{
			m_pcscOp.Hex2Bin(szTemp, szData, iLength);
			memset(szData, 0, sizeof(szData));

			des.TriDesDecrypt(CONTROL_KEY, 16, szTemp, iLength / 2, szData);

			memset(szTemp, 0, sizeof(szTemp));
			m_pcscOp.Bin2Hex(szTemp, szData, iLength / 2);

			iRet = RemoveDataPad(szTemp, &iLength);
			if (iRet)
			{
				return 1;
			}
			strTemp += (char*)szTemp;
			strTemp += "\n";
		}
		else
		{
			strTemp += (char*)szData;
			strTemp += "\n";
		}
	}
	strData.clear();
	strData.append(strTemp.GetBuffer(), strTemp.GetLength());
	return 0;
}

