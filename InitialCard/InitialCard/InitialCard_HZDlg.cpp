
// InitialCard_HZDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "InitialCard_HZ.h"
#include "InitialCard_HZDlg.h"
#include "afxdialogex.h"
//#include "mac.h"

//#include <boost\program_options\detail\utf8_codecvt_facet.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
/////////
////////

#define	PORT "USB1"
#define BAUD 2
#define CHECKSUM	0x69

#define CARD_PHASE	0 //1: 一期卡，需要补丁。 0：二期卡，不需补丁
#define CAK		"11223344556677889900112233445566"

#define DEFAULT_KEY	"FFFFFFFFFFFF"
#define DEFAULT_ACCESS	"FFFFFFFFFFFFFF078069FFFFFFFFFFFF"
#define SPEC_ACCESS		"FFFFFFFFFFFF7F078800FFFFFFFFFFFF"
#define CONTROL_KEY		(unsigned char*)"\x0C\x57\x5A\x62\x1A\x78\xCB\xEE\x84\x66\x32\x7A\x11\xEA\xB3\x15"

#define SETTING_FILE "PersoSet.xml"

#define TREATHR(hr)	if FAILED(hr) _com_issue_error(hr);

/////////////////////////////////////////////////////////////////////


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CInitialCard_HZDlg 对话框




CInitialCard_HZDlg::CInitialCard_HZDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInitialCard_HZDlg::IDD, pParent)
	, m_bStop(FALSE)
	, m_bStarted(FALSE)
	, strTemp(_T(""))
	, lBegin(0)
	, m_strUIDLog(_T(""))
	, m_isDbOpen(0)
	, m_iInitialErr(0)
	, m_iChangeUIDFlag(0)
	, m_iDevIndex(0)
	, m_iUID_Offset(0)
	, m_dwProtocol(0)
	, m_bIsSelectedCard(0)
	, m_isNewDB(0)
	, m_isReaderOpen(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_INFINITY);
	m_StrDisp = _T("");
	m_strSnBegin = _T("");
}

void CInitialCard_HZDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DISP, m_EdtDisp);
	DDX_Text(pDX, IDC_EDIT_DISP, m_StrDisp);
	DDX_Control(pDX, IDC_BTN_ERASECARD, m_BtnEraseCard);
	DDX_Control(pDX, IDC_BTN_CARDTEST, m_BtnTestCard);
	DDX_Control(pDX, IDC_EDIT_SN, m_EdtSnBegin);
	DDX_Text(pDX, IDC_EDIT_SN, m_strSnBegin);
	DDX_Control(pDX, IDC_BTN_START, m_BtnStart);
	DDX_Control(pDX, IDC_BTN_STOP, m_BtnStop);
	DDX_Control(pDX, IDC_BTN_CHANGE_UID, m_BtnChangeUID);
	DDX_Control(pDX, IDC_BTN_CHECK_UID, m_BtnGetUID);
	DDX_Control(pDX, IDC_BTN_RECORD_CSN, m_BtnCardType);
	DDX_Control(pDX, IDC_COMBO_PERSO_TYPE, m_cmbPersoType);
}

BEGIN_MESSAGE_MAP(CInitialCard_HZDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CInitialCard_HZDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CInitialCard_HZDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_CARDTEST, &CInitialCard_HZDlg::OnBnClickedBtnCardtest)
	ON_BN_CLICKED(IDC_BTN_ERASECARD, &CInitialCard_HZDlg::OnBnClickedBtnErasecard)
	ON_BN_CLICKED(IDC_BTN_PURCHASE, &CInitialCard_HZDlg::OnBnClickedBtnPurchase)
	ON_BN_CLICKED(IDC_BTN_CHECK_UID, &CInitialCard_HZDlg::OnBnClickedBtnCheckUid)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_BTN_CHANGE_UID, &CInitialCard_HZDlg::OnBnClickedBtnChangeUid)
ON_BN_CLICKED(IDC_BTN_RECORD_CSN, &CInitialCard_HZDlg::OnBnClickedBtnRecordCsn)
//ON_CBN_CLOSEUP(IDC_COMBO_PERSO_TYPE, &CInitialCard_HZDlg::OnCbnCloseupComboPersoType)
//ON_CBN_KILLFOCUS(IDC_COMBO_PERSO_TYPE, &CInitialCard_HZDlg::OnCbnKillfocusComboPersoType)
ON_CBN_CLOSEUP(IDC_COMBO_PERSO_TYPE, &CInitialCard_HZDlg::OnCbnCloseupComboPersoType)
ON_CBN_KILLFOCUS(IDC_COMBO_PERSO_TYPE, &CInitialCard_HZDlg::OnCbnKillfocusComboPersoType)
END_MESSAGE_MAP()


// CInitialCard_HZDlg 消息处理程序

BOOL CInitialCard_HZDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	int iRet = 0;

	SetWindFont();

	//配置信息
	iRet = GetInitSettings();
	if(iRet)
	{
		AfxMessageBox("读取配置文件失败!");
		exit(-3);
	}
	
	//自动连接读写器
	ConnectReader();

	//初始化数据库连接
	InitDatabase();

	//设置发卡类别
	SetPersoTypeCombo();

	//卡号设置
	m_EdtSnBegin.LimitText(10);
	m_EdtSnBegin.SetWindowTextA("3010000001");

	//varable initialize
	memset(m_szCardUID, 0, sizeof(m_szCardUID));
	memset(m_szATR, 0, sizeof(m_szATR));
	memset(m_szCardProperty, 0, sizeof(m_szCardProperty));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CInitialCard_HZDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CInitialCard_HZDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CInitialCard_HZDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CInitialCard_HZDlg::OnBnClickedBtnStart()
{
	// TODO: 在此添加控件通知处理程序代码
	THREADSTRUCT *_param = new THREADSTRUCT;

	m_StrDisp.Empty();

	if (!m_isReaderOpen)
	{
		int iRet = ConnectReader();
		if (iRet)
		{
			m_EdtDisp.SetWindowTextA("连接读写器失败, 请检查连接!");
			return;
		}
	}
	m_EdtDisp.SetWindowTextA("开始发卡！");

	m_bStop = 0;
	if(!m_bStarted)
	{
		_param->_this = this;
		AfxBeginThread(StartInitCardThread, _param);
		m_bStarted = 1;
	}

	m_BtnTestCard.EnableWindow(FALSE);
	m_BtnStart.EnableWindow(FALSE);
	m_BtnStop.EnableWindow(TRUE);
	
	m_BtnEraseCard.EnableWindow(FALSE);
	m_EdtSnBegin.EnableWindow(FALSE);
	m_BtnGetUID.EnableWindow(FALSE);
	m_BtnChangeUID.EnableWindow(FALSE);

	m_cmbPersoType.EnableWindow(FALSE);
}

void CInitialCard_HZDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码

	m_bStop = 1;
	m_bStarted = 0;
	m_iInitialErr = 0;
	
	m_BtnTestCard.EnableWindow(TRUE);
	m_BtnEraseCard.EnableWindow(TRUE);

	m_BtnGetUID.EnableWindow(TRUE);
	m_BtnChangeUID.EnableWindow(TRUE);
	m_cmbPersoType.EnableWindow(TRUE);
	m_EdtSnBegin.EnableWindow(TRUE);

	m_BtnStart.EnableWindow(TRUE);
	m_BtnStop.EnableWindow(FALSE);

	m_BtnStop.SetWindowTextA("结束发卡");
}

int CInitialCard_HZDlg::DisplayHint(CString strDisp)
{
	m_StrDisp += strDisp + "\r\n";
	m_EdtDisp.SetWindowTextA(m_StrDisp);

	m_EdtDisp.LineScroll(m_EdtDisp.GetLineCount());

	return 0;
}

// 发卡线程
UINT CInitialCard_HZDlg::StartInitCardThread(LPVOID param)
{
	THREADSTRUCT *ts = (THREADSTRUCT*) param;
	int iCount = 0, iRet = 0, iErrCount = 0;
	unsigned long lUID = 0;
	char szRotateUID[12];
	BOOL bMifareInited = 0;

	CString strTemp;

	//发卡循环
	while(!ts->_this->m_bStop)
	{
		Sleep(100);	
		ts->_this->m_iInitialErr = 0;
		iRet = ts->_this->SelectCard();
		if(iRet)
			continue;
		
		bMifareInited = 0;
		if(!ts->_this->strMifareInit.compare("ENABLE"))
		{
			iRet = ts->_this->ChangeMifareControlByte();
			if(iRet)
				goto CONTINUE;
			bMifareInited = 1;
		}
		if(!ts->_this->m_strDeviceType.compare("W8") || !ts->_this->strMifareInit.compare("DISABLE"))
		{
			iRet = ts->_this->UserCardReset();
			if(iRet)
				continue;
		}
		
		ts->_this->m_StrDisp.Empty();
		ts->_this->m_EdtDisp.SetWindowTextA(ts->_this->m_StrDisp);

		lUID = ts->_this->RotateUID(ts->_this->m_szCardUID);
		ts->_this->m_EdtSnBegin.GetWindowTextA(ts->_this->m_strSnBegin);
		memset(szRotateUID, 0, sizeof(szRotateUID));
		sprintf_s(szRotateUID, 11, "%u", lUID);
		ts->_this->m_strUIDLog.Format("INSERT INTO UidLog(CardType, FACTRY,INITIAL_DATE,ROTATE_UID,MIFARE_INITED,CSN,ATS,PATCH,SN, UID) Values('%d','MINGHUA','%s','%010s', '%d',", \
			ts->_this->m_cmbPersoType.GetCurSel(), ts->_this->m_szDate, szRotateUID, bMifareInited);

		iRet = ts->_this->ConstructCard();
		if(iRet)
			goto CONTINUE;

		ts->_this->m_strUIDLog.AppendFormat("'%s','%s','%d','%s','%s');", ts->_this->m_szDIV, ts->_this->m_szATS, ts->_this->m_iPatchLines,ts->_this->m_szSN, ts->_this->m_szCardUID);

		iCount ++;
		ts->_this->LogUID();
		ts->_this->m_StrDisp.Empty();
		strTemp.Format("发卡成功，请放下一张卡片！\r\n总发卡数：%d; 其中成功：%d; 失败：%d;", iCount + iErrCount, iCount, iErrCount);
		ts->_this->IncrementSN();
		ts->_this->DisplayHint(strTemp);
		if(!ts->_this->m_strDeviceType.compare("W8"))
		{
			ts->_this->BeepForSuccess();
		}
		continue;

CONTINUE:
		{
			iErrCount ++;
			ts->_this->m_iInitialErr = 1;
			strTemp.Format("发卡失败，请放下一张卡片！\r\n总发卡数：%d; 其中成功：%d; 失败：%d;", iCount + iErrCount, iCount, iErrCount);
			ts->_this->DisplayHint(strTemp);
			ts->_this->DisplayHint("CSN<%s>	UID<%s>", ts->_this->m_szDIV, ts->_this->m_szCardUID);
			//if(ts->_this->strPersoType.compare("GET_UID"))//increse sn number if perso
			//	ts->_this->IncrementSN();
			if(!ts->_this->m_strDeviceType.compare("W8"))
			{
				ts->_this->BeepForFail();
			}
			continue;
		}

	}//while loop
	
	ts->_this->DisplayHint("发卡结束;");
	AfxEndThread(0);
	return 0;
}

// 写入结构
int CInitialCard_HZDlg::ConstructCard(void)
{
	int iRet = 0, iPersoType = m_cmbPersoType.GetCurSel();
	unsigned int iLen = 0, iOffset = 0;
	unsigned char szRetST[5], szRetData[128];
	string strCommand;
	
	m_iPatchLines = 0;

	//user card
	if (iPersoType != PSAM_CARD)
	{
		iRet = InitializeMF();
		if (iRet)
			return iRet;
	}

	//common apdus
	iLen = 0;
	iOffset = 0;
	while((iLen = strAPDU.find(0x0a, iOffset)) != string::npos)
	{
		strCommand.clear();
		strCommand.append(strAPDU.c_str() + iOffset, iLen - iOffset);

		iRet = CheckCommand(strCommand);
		if(iRet)
			return iRet;

		iOffset = iLen + 1;
	}

	//尾行
	if(iLen == string::npos && iOffset  < strAPDU.length())
	{
		strCommand.clear();
		strCommand.append(strAPDU.c_str() + iOffset, strAPDU.length() - iOffset);

		iRet = CheckCommand(strCommand);
		if(iRet)
			return iRet;
	}

	if (iPersoType == PSAM_CARD)
	{
		iRet = WritePSAMCardKeys();
		if (iRet)
			return iRet;
	}
	else
	{
		iRet = WriteUserCardKeys();
		if (iRet)
			return iRet;
	}


	//log ats
	if(!m_strDeviceType.compare("PCSC") && !strInterface.compare("CONTACTLESS") && m_strDeviceName.find("SDI") != string::npos)
	{
		//ATS
		memset(szRetST, 0, sizeof(szRetST));
		memset(szRetData, 0, sizeof(szRetData));
		iRet = ICC_SendCommand((BYTE*)"FFCC00000193", 12, szRetST, szRetData);
		if(iRet)
		{
			DisplayHint("取ATS信息失败.");
			return 1;
		}
		memset(m_szATS, 0, sizeof(m_szATS));
		memcpy(m_szATS, szRetData, strlen((char*)szRetData));

		//UID
		memset(szRetST, 0, sizeof(szRetST));
		memset(szRetData, 0, sizeof(szRetData));
		iRet = ICC_SendCommand((BYTE*)"FFCA000004", 10, szRetST, szRetData);
		if (iRet)
		{
			DisplayHint("取UID信息失败.");
			return 1;
		}
		memset(m_szCardUID, 0, sizeof(m_szCardUID));
		memcpy(m_szCardUID, szRetData, strlen((char*)szRetData));

	}

	if (!m_strDeviceType.compare("PCSC"))
	{
		iRet = m_pcscOp.PowerOffCard(m_iDevIndex);
	}

	return 0;
}

void CInitialCard_HZDlg::OnBnClickedBtnCardtest()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_bStarted == 1)
		return;

	THREADSTRUCT *_param = new THREADSTRUCT;

	m_StrDisp.Empty();
	if (!m_isReaderOpen)
	{
		int iRet = ConnectReader();
		if (iRet)
		{
			m_EdtDisp.SetWindowTextA("连接读写器失败, 请检查连接!");
			return;
		}
	}
	m_EdtDisp.SetWindowTextA("开始检测卡片！");

	m_bStop = 0;
	if(!m_bStarted)
	{
		_param->_this = this;
		AfxBeginThread(StartCheckCardConstruct, _param);
		m_bStarted = 1;
	}

	m_BtnTestCard.EnableWindow(FALSE);
	
	m_BtnStart.EnableWindow(FALSE);
	m_cmbPersoType.EnableWindow(FALSE);
	m_BtnStop.EnableWindow(TRUE);
	m_BtnStop.SetWindowTextA("结束测试");

	m_BtnGetUID.EnableWindow(FALSE);
	m_BtnChangeUID.EnableWindow(FALSE);
	
	m_BtnEraseCard.EnableWindow(FALSE);

	m_EdtSnBegin.EnableWindow(FALSE);
}

// 添加MifareKey
int CInitialCard_HZDlg::AppendMifareKey(void)
{
	int iRet = 0;
	unsigned char szRetST[5], szRetData[128];
	unsigned char szCommand[256];

	iRet = UserCardReset();
	if(iRet)
		return iRet;

	memset(szRetST, 0, sizeof(szRetST));
	memset(szRetData, 0, sizeof(szRetData));
	iRet = m_eyeOp.ICC_SendCommand(m_eyeOp.USERCARD, (unsigned char*)"00A40000023F01", 14, szRetST, szRetData);
	if(iRet < 0 || iRet > 255)
	{
		strTemp.Format("Select 3f01 failed; Please check card status;");
		DisplayHint(strTemp);
		return 6;
	}

	iRet = ExternalAuth_MK();
	if(iRet)
		return iRet;

	//append mifare key
	memset(szCommand, 0, sizeof(szCommand));
	for(int i = 0; i != 16; ++i)
	{
		sprintf_s((char*)szCommand, sizeof(szCommand), "80DA%02X400B014040EC7B9CB278FA39DD", i);
		szCommand[32] = 0;

		iRet = m_eyeOp.ICC_SendCommand(m_eyeOp.USERCARD, szCommand, 32, szRetST, szRetData);
		if(iRet < 0 || iRet > 255)
		{
			strTemp.Format("添加MRW密钥失败;\r\nSW: %s, return: %s;", szRetST, szRetData);
			DisplayHint(strTemp);
			return 6;
		}
	}

	return 0;
}

// 修改默认控制字
int CInitialCard_HZDlg::ChangeMifareControlByte(void)
{
	int iRet = 0, iLen = 0;
	unsigned char szControlData[40];
	unsigned char szControlByte[33];

	memset(szControlData, 0, sizeof(szControlData));
	memset(szControlByte, 0, sizeof(szControlByte));

	//假设为默认控制字
	memcpy(szControlByte + 6, "\xFF\x07\x80", 3);

	iLen = 32;
	memcpy(szControlData, DEFAULT_ACCESS, iLen);//FF 07 80 69
	//memcpy(szControlData, SPEC_ACCESS, iLen);
	m_eyeOp.SwitchDataMode(szControlData, &iLen, m_eyeOp.HEX2ASC);

	if(!m_strDeviceType.compare("W8"))
	{
		for(int i = 0; i != 64; ++i)//0、1扇区不修改
		{
			if(i%4 == 3)
			{
				if(i == 3)
				{//读取0扇区控制字
					iRet = m_eyeOp.Dev_MifareAuthenticate(0x00, i, (unsigned char*)DEFAULT_KEY);
					if(iRet)
					{
						strTemp.Format("认证扇区密钥 %d 失败;\r\nReturn value: %d;", i/4, iRet);
						DisplayHint(strTemp);
						UserCardReset();
						BeepForFail();
						return 9;
					}

					iRet = m_eyeOp.Dev_MifareReadBlock(i, szControlByte);
					if(iRet)
					{
						strTemp.Format("读扇区控制字 %d 失败;\r\nReturn value: %d;", i/4, iRet);
						DisplayHint(strTemp);
						UserCardReset();
						BeepForFail();
						return 9;
					}
				}

				//if(!memcmp(szControlByte + 6, "\xFF\x07\x80", 3))
				if(!memcmp(szControlByte + 6, "\x7F\x07\x88", 3))
				{

					iRet = m_eyeOp.Dev_MifareAuthenticate(0x01, i, (unsigned char*)DEFAULT_KEY);//认证B密码	
					//iRet = m_eyeOp.Dev_MifareAuthenticate(0x00, i, (unsigned char*)DEFAULT_KEY);//修改成7F078800
					if(iRet)
					{
						strTemp.Format("认证扇区密钥 %d 失败;\r\nReturn value: %d;", i/4, iRet);
						DisplayHint(strTemp);
						UserCardReset();
						BeepForFail();
						return 9;
					}

					iRet = m_eyeOp.Dev_MifareWriteBlock(i, szControlData);
					if(iRet)
					{
						strTemp.Format("写块 %d 数据失败; return value: %d;", i, iRet);
						DisplayHint(strTemp);
						UserCardReset();
						BeepForFail();
						return 9;
					}
				}
			}
		}
	}
	else
	{
		iRet = UserCardReset();
		if(iRet)
		{
			DisplayHint("卡片上电失败!");
			return iRet;
		}

		iRet = ChangeMifareControlWord();
		if(iRet)
			return iRet;
	}

	DisplayHint("修改卡片控制字完成!");
	return 0;
}
//PCSC 机具修改Mifare 控制字
int CInitialCard_HZDlg::ChangeMifareControlWord(void)
{
	int iRet = 0, iLen = 0;
	unsigned char szCommand[256];
	unsigned char szRetST[5], szRetData[512];

	//Load A/B key
	memset(szCommand, 0, sizeof(szCommand));
	memset(szRetST, 0, sizeof(szRetST));
	memset(szRetData, 0, sizeof(szRetData));
	iLen = strlen("FF82006106FFFFFFFFFFFF");
	memcpy(szCommand, "FF82006106FFFFFFFFFFFF", iLen);
	iRet = m_pcscOp.ICC_SendCommand(m_iDevIndex, m_pcscOp.PROTOCOL_T1, szCommand, iLen, szRetST, szRetData);
	if(iRet != 0)
	{
		if(!memcmp(szRetST, "6E00", 4))
		{
			DisplayHint("卡片为纯CPU卡，跳过控制字修改!");
			return 0;
		}
		strTemp.Format("Send command failed:\r\n%s;", m_pcscOp.m_strError);
		DisplayHint(strTemp);
		return iRet;
	}

	//change mifare access words
	for(int i = 0; i != 64; ++i)
	{
		if(i%4 == 3)
		{
			//
			memset(szCommand, 0, sizeof(szCommand));
			memset(szRetST, 0, sizeof(szRetST));
			memset(szRetData, 0, sizeof(szRetData));
			sprintf_s((char*)szCommand, sizeof(szCommand), "FF860000050100%02X6106", i);
			szCommand[20] = 0;
			iLen = 20;
			iRet = m_pcscOp.ICC_SendCommand(m_iDevIndex, m_pcscOp.PROTOCOL_T1, szCommand, iLen, szRetST, szRetData);
			if(iRet != 0)
			{
				strTemp.Format("Send command failed: %s;", m_pcscOp.m_strError);
				DisplayHint(strTemp);
				return iRet;
			}

			//
			memset(szCommand, 0, sizeof(szCommand));
			memset(szRetST, 0, sizeof(szRetST));
			memset(szRetData, 0, sizeof(szRetData));
			sprintf_s((char*)szCommand, sizeof(szCommand), "FFD600%02X10FFFFFFFFFFFFFF078069FFFFFFFFFFFF", i);
			szCommand[42] = 0;
			iLen = 42;
			iRet = m_pcscOp.ICC_SendCommand(m_iDevIndex, m_pcscOp.PROTOCOL_T1, szCommand, iLen, szRetST, szRetData);
			if(iRet != 0)
			{
				if(!memcmp(szRetST, "6982", 4))//已修改成功
					continue;
				strTemp.Format("Send command failed: %s;", m_pcscOp.m_strError);
				DisplayHint(strTemp);
				return iRet;
			}
		}
	}

	DisplayHint("修改Mifare控制字到默认状态完成!");

	return 0;
}

// check control words
int CInitialCard_HZDlg::CheckControlWords(void)
{
	int iRet = 0, iLen = 16;
	unsigned char szBlockData[40];

	memset(szBlockData, 0, sizeof(szBlockData));
	for(int i = 0; i != 64; ++i)
	{
		if(i%4 == 3)
		{
			iRet = m_eyeOp.Dev_MifareAuthenticate(0x00, i, (unsigned char*)DEFAULT_KEY);
			if(iRet)
			{
				strTemp.Format("认证扇区<%d>密钥失败;\r\nReturn value: %d;", i/4, iRet);
				DisplayHint(strTemp);
				m_eyeOp.Dev_MifareHalt();
				return 10;
			}

			iRet = m_eyeOp.Dev_MifareReadBlock(i,szBlockData);
			if(iRet)
			{
				strTemp.Format("读取控制字失败;\r\nReturn value: %d;", iRet);
				DisplayHint(strTemp);
				m_eyeOp.Dev_MifareHalt();
				return 10;
			}
			iLen = 16;
			m_eyeOp.SwitchDataMode(szBlockData, &iLen, m_eyeOp.ASC2HEX);
			if(memcmp(szBlockData + 12, "FF078069", 8))
			{
				strTemp.Format("控制非默认控制字;\r\n当前控制位信息为: %s;", szBlockData);
				DisplayHint(strTemp);
				m_eyeOp.Dev_MifareHalt();
				return 10;
			}
		}

	}

	return 0;
}

//清卡
void CInitialCard_HZDlg::OnBnClickedBtnErasecard()
{
	// TODO: 在此添加控件通知处理程序代码
	int iRet = 0;
	unsigned int iLen = 0, iOffset = 0;


	m_StrDisp.Empty();
	if (!m_isReaderOpen)
	{
		iRet = ConnectReader();
		if (iRet)
		{
			m_EdtDisp.SetWindowTextA("连接读写器失败, 请检查连接!");
			return;
		}
	}

	m_EdtDisp.SetWindowTextA("开始清卡 ...");


	//强制对卡上电，不做寻卡步骤
	m_bIsSelectedCard = 1;

	iRet = UserCardReset();
	if(iRet)
	{
		DisplayHint("卡片复位失败;\r\n退出处理程序 ...");
		return;
	}

	string strCommand;

	iLen = 0;
	while((iLen = strRecoveryAPDU.find(0x0a, iOffset)) != string::npos)
	{
		strCommand.clear();
		strCommand.append(strRecoveryAPDU.c_str() + iOffset, iLen - iOffset);

		iRet = CheckCommand(strCommand);
		if(iRet)
		{
			DisplayHint("发送指令失败!	指令：\r\n%s", strCommand.c_str());
			return;
		}

		iOffset = iLen + 1;
	}

	if(iLen == string::npos && iOffset  < strRecoveryAPDU.length())
	{
		strCommand.clear();
		strCommand.append(strRecoveryAPDU.c_str() + iOffset, strRecoveryAPDU.length() - iOffset);

		iRet = CheckCommand(strCommand);
		if(iRet)
		{
			DisplayHint("发送指令失败!	指令：\r\n%s", strCommand.c_str());
			DisplayHint("清卡失败!");
			return;
		}
	}

	DisplayHint("清卡成功！");
	if(!m_strDeviceType.compare("W8"))
		BeepForSuccess();
}

UINT CInitialCard_HZDlg::StartPurchase(LPVOID param)
{
	THREADSTRUCT *ts;
	ts = (THREADSTRUCT*)param;
	int iRet = 0;

	while(!ts->_this->m_bStop)
	{
		Sleep(500);
		ts->_this->m_eyeOp.m_iSelectCardMode = 1;
		iRet = ts->_this->UserCardReset();
		if(iRet)
			continue;
		ts->_this->m_eyeOp.m_iSelectCardMode = 0;

		iRet = ts->_this->Purchase();
		if(iRet)
			goto CONTINUE;

		ts->_this->DisplayHint("消费成功!");
		ts->_this->BeepForSuccess();
		continue;

CONTINUE:
		{
			ts->_this->DisplayHint("消费成功!");
			ts->_this->m_eyeOp.Dev_Beep(3,1,2);
			continue;
		}
	}

	ts->_this->DisplayHint("结束消费循环!");
	AfxEndThread(0);
	return 0;
}

UINT CInitialCard_HZDlg::StartCheckCardConstruct(LPVOID param)
{
	THREADSTRUCT *ts;
	ts = (THREADSTRUCT*)param;
	int iRet = 0;


	while(!ts->_this->m_bStop)
	{
		Sleep(100);

		iRet = ts->_this->ConstructCheck();
		if(iRet)
			//goto CONTINUE;
			continue;
	}

	ts->_this->DisplayHint("卡片结构检测结束!");
	AfxEndThread(0);
	return 0;
}

// 0值消费
int CInitialCard_HZDlg::Purchase(void)
{
	return 0;
}

void CInitialCard_HZDlg::OnBnClickedBtnPurchase()
{
	// TODO: 在此添加控件通知处理程序代码
	THREADSTRUCT *_param = new THREADSTRUCT;

	m_bStop = 0;
	if(!m_bStarted)
	{
		_param->_this = this;
		AfxBeginThread(StartPurchase, _param);
		m_bStarted = 1;
	}

	m_BtnTestCard.EnableWindow(FALSE);

	m_BtnEraseCard.EnableWindow(FALSE);
}

void CInitialCard_HZDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if(m_pConn->State == 1)
		m_pConn->Close();
}

int CInitialCard_HZDlg::CheckUID(void)
{
	CString strSqlCmd;
	int iCount = 0;
	long lIndex = 0;
	VARIANT varSN;

	strSqlCmd = "SELECT SN FROM UidLog WHERE UID = '";
	strSqlCmd += (char*)m_szCardUID;
	strSqlCmd += "';";

	HRESULT hr = S_OK;
	_variant_t varAffected, varCount, varIndex = (long)0;

	try
	{
		hr = pRecordset.CreateInstance("ADODB.Recordset");
		if(SUCCEEDED(hr))
		{
			pRecordset->Open(_bstr_t(strSqlCmd.GetBuffer()), m_pConn.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);
			if((iCount = pRecordset->RecordCount) <= 0)
			{
				DisplayHint("未找到相应数据!");
				return 1;
			}
			else if(iCount > 1)
			{
				DisplayHint("找到多条<%d>数据!", iCount);
				lIndex = 1;
				while(!pRecordset->EndOfFile)
				{
					varSN = pRecordset->GetCollect("SN");
					DisplayHint("SN<%d>:	%s", lIndex, (LPCTSTR)(_bstr_t)varSN.bstrVal);
					pRecordset->MoveNext();
					++lIndex;
				}
				DisplayHint("请联系管理员对该数据进行处理!");
				pRecordset->Close();
				return 1;
			}
			else
			{
				varSN = pRecordset ->GetCollect("SN");
				DisplayHint("请确认当前卡号： >>	%s	<<", (LPCTSTR)(_bstr_t)varSN.bstrVal);

				m_iChangeUIDFlag = 1;
			}

		}
		else
			_com_issue_error(hr);
	}
	catch(_com_error &err)
	{
		DisplayHint("查询数据库中数据失败!\r\n错误：%s", err.ErrorMessage());
		return 1;
	}
	return 0;
}

void CInitialCard_HZDlg::OnBnClickedBtnCheckUid()
{
	// TODO: 在此添加控件通知处理程序代码
	int iRet = 0;

	m_StrDisp.Empty();
	m_EdtDisp.SetWindowTextA("");

	//iRet = SelectCard();
	//if(iRet)
	//{
	//	DisplayHint("未找到卡片!");
	//	return;
	//}
	m_bIsSelectedCard = 1;

	iRet = UserCardReset();
	if(iRet)
	{
		DisplayHint("复位失败!");
		return;
	}

	iRet = CheckUID();
	if(iRet)
	{
		if(m_strDeviceType.find("W8") != string::npos)
			BeepForFail();
	}
	else
	{
		if(m_strDeviceType.find("W8") != string::npos)
			BeepForSuccess();
	}
		


}

HBRUSH CInitialCard_HZDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔

	if(pWnd->GetDlgCtrlID() == IDC_EDIT_SN)
		pDC->SetTextColor(RGB(0,0,255));
	if(pWnd->GetDlgCtrlID() == IDC_EDIT_DISP && m_iInitialErr == 1)
		pDC->SetTextColor(RGB(255, 0,0 ));
	if(pWnd->GetDlgCtrlID() == IDC_EDIT_DISP && m_iInitialErr == 0)
		pDC->SetTextColor(RGB(0, 0, 0 ));
	//hbr = m_brush;

	return hbr;
}

void CInitialCard_HZDlg::OnBnClickedBtnChangeUid()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_iChangeUIDFlag == 0)
	{
		DisplayHint("当前没有需要修改的UID数据");
		return;
	}
/*
	CDaoDatabase db;

	db.Open(m_szFilePath);
	CDaoRecordset rs(&db);
	CDaoTableDef td(&db);
	COleVariant key;
	COleVariant varSN;
	CString strLogSN;

	td.Open("UidLog");
	rs.Open(&td);
	rs.SetCurrentIndex(_T("indexUID"));
	
	m_EdtSnBegin.GetWindowTextA(m_strSnBegin);
	key.vt = VT_BSTRT;

	key = strUID;

	if(rs.Seek(_T("="), &key))
	{
		DisplayHint("开始修改该卡对应的卡号信息...");

		rs.Edit();
		varSN = m_strSnBegin;
		rs.SetFieldValue(_T("SN"), (const char*)m_strSnBegin);
		rs.Update();

		DisplayHint("修改完成!");

	}
	else
	{
		m_iChangeUIDFlag = 0;
		DisplayHint("未找到该序列号的记录!");
		return;
	}

	rs.Close();
	td.Close();
	db.Close();
	
	m_iChangeUIDFlag = 0;*/

	
	m_EdtSnBegin.GetWindowTextA(m_strSnBegin);
	try
	{
	pRecordset->MoveFirst();
	pRecordset->PutCollect("SN", _bstr_t(m_strSnBegin.GetBuffer()));

	HRESULT hr = pRecordset->Update();
	if(SUCCEEDED(hr))
	{
		DisplayHint("修改完成!");
		hr = pRecordset->Close();
	}
	else
		_com_issue_error(hr);
	}
	catch(_com_error &err)
	{
		DisplayHint("修改过程中出现错误：\t%s", err.ErrorMessage());
	}
	m_iChangeUIDFlag = 0;
}

//sort card's category
void CInitialCard_HZDlg::OnBnClickedBtnRecordCsn()
{
	int iRet = 0;

	ClearDisplay();
	if (!m_isReaderOpen)
	{
		int iRet = ConnectReader();
		if (!iRet)
		{
			m_EdtDisp.SetWindowTextA("连接读写器失败, 请检查连接!");
			return;
		}
	}


	iRet = SelectCard();
	if(iRet)
	{
		DisplayHint("未寻到卡或卡片因防冲突原因而移出操作序列, 请放置/重放置卡片!");
		return;
	}


	iRet = UserCardReset();
	if(iRet)
	{
		DisplayHint("卡片复位失败!");
		return;
	}

	string strATS;
	strATS = (char*)m_szATS;

	if(!strInterface.compare("CONTACT"))
	{//contact card or contact interface
		if(strATS.find("8638") != string::npos && strATS.find("3B6C") != string::npos)
		{
			DisplayHint("卡片ATR:	%s\r\n\r\n卡片型号为：SC-S-X 系列接触式卡片!", m_szATS);
			return;
		}
		
		if(strATS.find("3468") != string::npos && strATS.find("3B7B") != string::npos)
		{
			DisplayHint("卡片ATR:	%s\r\n\r\n卡片型号为：SC-NXP-ETC 系列双界面卡片!", m_szATS);
			return;
		}
	}
	else
	{
		if(strATS.find("8638") != string::npos)
		{
			unsigned char szRetSW[5], szRetData[128];
			memset(szRetSW, 0, sizeof(szRetSW));
			memset(szRetData, 0, sizeof(szRetData));

			if(strATS.find("12061903") != string::npos)
			{
				iRet = ICC_SendCommand((BYTE*)"84AD000004E4CE7C1B", 18, szRetSW, szRetData);
				if(iRet < 0)
				{
					DisplayHint("卡片ATS:	%s\r\n\r\n卡片型号为：SC-MOC-01\r\nV2 杭州公交专用版!", m_szATS);
					return;
				}
				DisplayHint("卡片ATS:	%s\r\n\r\n卡片型号为：SC-MOC-01\r\nV3 昆明公交\\福州自行车\\杭州公交适用版!", m_szATS);
			}
			else if(strATS.find("12112910") != string::npos)
			{
				DisplayHint("卡片ATS:	%s\r\n\r\n卡片型号为：SC-MOC-01\nV4 杭州公交最终版!", m_szATS);
			}
			else
			{
				DisplayHint("卡片ATS:	%s\r\n\r\n未指定卡片型号!", m_szATS);
			}
			return;
		}
		else
			DisplayHint("卡片ATS:	%s\r\n\r\n未指定卡片型号!", m_szATS);
	}
}


int CInitialCard_HZDlg::ICC_SendCommand(unsigned char* pszCommand, int iCmdLen, unsigned char* pszRetSW, unsigned char* pszRetData)
{
	if(!m_strDeviceType.compare("W8"))
	{
		return m_eyeOp.ICC_SendCommand(m_eyeOp.USERCARD, pszCommand, iCmdLen, pszRetSW, pszRetData);
	}
	else if(!m_strDeviceType.compare("PCSC"))
	{
		return m_pcscOp.ICC_SendCommand(m_iDevIndex, m_pcscOp.dwActiveProtocol, pszCommand, iCmdLen, pszRetSW, pszRetData);
	}
	else
		return 1;

	return 0;
}

// 卡片检测
int CInitialCard_HZDlg::ConstructCheck(void)
{
	int iRet = 0, iLen = 0;

	m_StrDisp.Empty();

	iRet = SelectCard();
	if(iRet)
	{
		//DisplayHint("寻卡失败;\r\n退出处理流程;");
		//if(!m_strDeviceType.compare("W8"))
		//	BeepForFail();
		return 1;
	}

	iRet = UserCardReset();
	if(iRet)
	{
		DisplayHint("卡片复位失败;\r\n退出处理流程;");
		if(!m_strDeviceType.compare("W8"))
			m_eyeOp.Dev_Beep(2, 1, 2);
		return 1;
	}

	string strCommand;

	iLen = 0;
	int iOffset = 0;
	while((iLen = strChkConstructAPDU.find(0x0a, iOffset)) != string::npos)
	{
		if(iLen == 0)
			continue;
		strCommand.clear();
		strCommand.append(strChkConstructAPDU.c_str() + iOffset, iLen - iOffset);

		iRet = CheckCommand(strCommand);
		if(iRet)
		{
			m_iInitialErr = 1;
			DisplayHint("失败, 该卡未建卡片结构!");
			if(!m_strDeviceType.compare("W8"))
				m_eyeOp.Dev_Beep(2, 1, 2);
			return 1;
		}

		iOffset = iLen + 1;
	}

	iLen = strChkConstructAPDU.length();
	if(iLen - iOffset >= 10)
	{
		strCommand.clear();
		strCommand.append(strChkConstructAPDU.c_str() + iOffset, iLen - iOffset);

		iRet = CheckCommand(strCommand);
		if(iRet)
		{
			m_iInitialErr = 1;
			DisplayHint("失败, 该卡片结构不完整!");
			if(!m_strDeviceType.compare("W8"))
				m_eyeOp.Dev_Beep(2, 1, 2);
			return 1;
		}
	}

	if (m_cmbPersoType.GetCurSel() != PSAM_CARD)
	{
		unsigned char szRetSW[5], szRetData[64];

		memset(szRetSW, 0, sizeof(szRetSW));
		memset(szRetData, 0, sizeof(szRetData));
		iRet = ICC_SendCommand((unsigned char*)"00B0000001", 10, szRetSW, szRetData);
		if (iRet)
		{
			DisplayHint("读取卡类型失败!");
			DisplayHint("卡片结构建立有误!\r\n 请确认是否选择错误的卡类型!");
			return 1;
		}

		if (m_cmbPersoType.GetCurSel() != (szRetData[1] - 0x30))
		{
			DisplayHint("卡片类型有误!\r\n 请确认是否选择错误的卡类型!\r\n期望卡类型为：%d, 实际卡类型为：%d", m_cmbPersoType.GetCurSel(), szRetData[1] - 0x30);
			return 1;
		}
	}

	m_iInitialErr = 0;
	//CheckUID();
	DisplayHint("检测完毕，卡片结构已经完全建立!\r\n%.8s\r\n", m_szCardUID);
	if(!m_strDeviceType.compare("W8"))
		BeepForSuccess();

	return 0;
}

int CInitialCard_HZDlg::SetMifareAccInit(void)
{
	int iRet = 0, iLen = 0;
	unsigned char szControlData[40];

	if(m_strDeviceType.compare("PCSC") != string::npos)
	{
		return 0;
	}

	iRet = SelectCard();
	if(iRet)
	{
		DisplayHint("寻卡失败!");
		if(!m_strDeviceType.compare("W8"))
			BeepForFail();
		return 1;
	}

	memset(szControlData, 0, sizeof(szControlData));

	iLen = 32;
	memcpy(szControlData, SPEC_ACCESS, iLen);
	m_eyeOp.SwitchDataMode(szControlData, &iLen, m_eyeOp.HEX2ASC);

	for(int i = 0; i != 64; ++i)
	{
		if(i%4 == 3)
		{
			iRet = m_eyeOp.Dev_MifareAuthenticate(0x00, i, (unsigned char*)DEFAULT_KEY);
			if(iRet)
			{
				strTemp.Format("认证扇区密钥 %d 失败;\r\nReturn value: %d;", i/4, iRet);
				DisplayHint(strTemp);
				UserCardReset();
				return 1;
			}

			iRet = m_eyeOp.Dev_MifareWriteBlock(i, szControlData);
			if(iRet)
			{
				strTemp.Format("写块 %d 数据失败; return value: %d;", i, iRet);
				DisplayHint(strTemp);
				UserCardReset();
				return 1;
			}
		}
	}

	return 0;
}

int CInitialCard_HZDlg::GetSN(unsigned char* pszUID_LSB, unsigned char* pszSN, int& iSNLength)
{
	CString strSqlCmd;
	int iCount = 0;
	long lIndex = 0;
	VARIANT varSN;
	_RecordsetPtr pRecordset;

	strSqlCmd = "SELECT SN FROM SN_DATA WHERE UID_LSB = '";
	strSqlCmd.AppendFormat("%.16s", pszUID_LSB);
	strSqlCmd += "';";

	HRESULT hr = S_OK;
	_variant_t varAffected, varCount, varIndex = (long)0;

	try
	{
		hr = pRecordset.CreateInstance("ADODB.Recordset");
		if(SUCCEEDED(hr))
		{
			pRecordset->Open(_bstr_t(strSqlCmd.GetBuffer()), m_pConn.GetInterfacePtr(), adOpenStatic, adLockOptimistic, adCmdText);
			if((iCount = pRecordset->RecordCount) <= 0)
			{
				DisplayHint("未找到该卡对应序号数据!");
				pRecordset->Close();
				return 1;
			}
			else if(iCount > 1)
			{
				DisplayHint("找到多条<%d>数据!", iCount);
				lIndex = 1;
				while(!pRecordset->EndOfFile)
				{
					//varSN = pRecordset->GetCollect(_variant_t(lIndex));
					varSN = pRecordset->GetCollect("SN");
					DisplayHint("SN<%d>:	%s", lIndex, (LPCTSTR)(_bstr_t)varSN.bstrVal);
					pRecordset->MoveNext();
					++lIndex;
				}
				DisplayHint("请联系管理员对该数据进行处理!");
				pRecordset->Close();
				return 1;
			}
			else
			{
				varSN = pRecordset ->GetCollect("SN");
				sprintf_s((char*)pszSN, 40, "%s", (LPCTSTR)(_bstr_t)varSN.bstrVal);
				iSNLength = strlen((char*)pszSN);
				m_strSnBegin.Empty();
				m_strSnBegin.Format((char*)pszSN);
				m_EdtSnBegin.SetWindowTextA(m_strSnBegin);
				DisplayHint("\r\n当前卡片序号为：%s\r\n", pszSN);

				pRecordset->Close();
			}

		}
		else
			_com_issue_error(hr);
	}
	catch(_com_error &err)
	{
		DisplayHint("查询数据库中数据失败!\r\n错误：%s", err.ErrorMessage());
		pRecordset->Close();
		return 1;
	}
	return 0;
}

