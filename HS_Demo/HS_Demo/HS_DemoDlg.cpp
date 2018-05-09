
// HS_DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HS_Demo.h"
#include "HS_DemoDlg.h"
#include "afxdialogex.h"

#include "SerialPort.h"
#include "HEX2BIN.hpp"
#include "ErrLog.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RF_CARD		0
#define SAM_CARD	1

#define SELECT	0
#define COM1	1
#define COM2	2
#define COM3	3
#define COM4	4
#define COM5	5

#define EF05	0
#define EF06	1
#define EF07	2
#define EF08	3

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


// CHS_DemoDlg 对话框



CHS_DemoDlg::CHS_DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHS_DemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHS_DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_APDU, m_edtAPDU);
	DDX_Control(pDX, IDC_EDIT_PROTOCOL, m_edtProtocol);
	DDX_Control(pDX, IDC_COMBO_CARD_SEAT, m_cmbCardSeat);
	DDX_Control(pDX, IDC_BUTTON_RST, m_btnResetProtocol);
	DDX_Control(pDX, IDC_COMBO_PORT, m_cmbPort);
	DDX_Control(pDX, IDC_EDIT_ADMIN_NO, m_edtAdminCardN);
	DDX_Control(pDX, IDC_EDIT_USER_CARD, m_edtCardNO);
	DDX_Control(pDX, IDC_COMBO_FILE_ID, m_cmbFileID);
	DDX_Control(pDX, IDC_EDIT_DISP, m_edtDisp);
}

BEGIN_MESSAGE_MAP(CHS_DemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_RST, &CHS_DemoDlg::OnBnClickedButtonRst)
	ON_BN_CLICKED(IDC_BUTTON_GEN_PROTOCOL, &CHS_DemoDlg::OnBnClickedButtonGenProtocol)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_PORT, &CHS_DemoDlg::OnBnClickedButtonOpenPort)
END_MESSAGE_MAP()


// CHS_DemoDlg 消息处理程序

BOOL CHS_DemoDlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_cmbCardSeat.InsertString(RF_CARD, "非接触卡座");
	m_cmbCardSeat.InsertString(SAM_CARD, "接触式卡座");
	m_cmbCardSeat.SetCurSel(0);


	m_cmbPort.InsertString(SELECT, "请选择串口");
	m_cmbPort.InsertString(COM1, "COM1");
	m_cmbPort.InsertString(COM2, "COM2");
	m_cmbPort.InsertString(COM3, "COM3");
	m_cmbPort.InsertString(COM4, "COM4");
	m_cmbPort.InsertString(COM5, "COM5");
	m_cmbPort.SetCurSel(SELECT);

	m_cmbFileID.InsertString(EF05, "EF05");
	m_cmbFileID.InsertString(EF06, "EF06");
	m_cmbFileID.InsertString(EF07, "EF07");
	m_cmbFileID.InsertString(EF08, "EF08");
	m_cmbFileID.InsertString(EF05);

	m_edtAPDU.SetWindowTextA("0020000003123456");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHS_DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHS_DemoDlg::OnPaint()
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
HCURSOR CHS_DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CHS_DemoDlg::OnBnClickedButtonRst()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_cmbCardSeat.GetCurSel() == RF_CARD)
	{//非接触卡座
		m_edtProtocol.SetWindowTextA("AA2000053000000000BF");
	}
	else
	{//as SAM Card seat
		m_edtProtocol.SetWindowTextA("AA2000052200000000AD");
	}
}


void CHS_DemoDlg::OnBnClickedButtonGenProtocol()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strTemp;
	unsigned char szProtocolData[256];

	m_edtAPDU.GetWindowTextA(strTemp);

	memset(szProtocolData, 0, sizeof(szProtocolData));
	ConstructProtocol(strTemp, m_cmbCardSeat.GetCurSel(), szProtocolData);

	m_edtProtocol.SetWindowTextA((LPCTSTR)szProtocolData);
}


// Apdu
int CHS_DemoDlg::ConstructProtocol(CString strAPDU, int iCardSeat, unsigned char* pszProtocol)
{
	unsigned char szProtocol[256];

	memset(szProtocol, 0, sizeof(szProtocol));

	//Byte [3] is Protocol length byte
	//Byte [4] is card seat select 0x30:RF  0x22:SAM
	//Byte [11] APDU length byte
	memcpy(szProtocol, "\xAA\x21\x00\x00\x30\x00\x00\x00\x00\x00\x00", 11);

	//set card seat
	if (iCardSeat == RF_CARD)
	{//非接触卡座
		szProtocol[4] = 0x30;
	}
	else
	{
		szProtocol[4] = 0x22;
	}

	//Byte [11] APDU length byte
	int iApduLen = strAPDU.GetLength() / 2;
	szProtocol[11] = (unsigned char)(iApduLen);

	//Byte [3] is Protocol length byte  APDU len +7 + 1 length byte + 1 cc
	szProtocol[3] = (unsigned char)(szProtocol[11] + 9);

	Hex2Bin(szProtocol + 12, (unsigned char*)strAPDU.GetBuffer(), iApduLen * 2);

	//generate crc
	iApduLen += 13;
	szProtocol[iApduLen - 1] = 0x00;
	szProtocol[iApduLen] = 0x00;
	for (int i = 0; i != iApduLen; ++i)
	{
		szProtocol[iApduLen] ^= szProtocol[i];
	}
	iApduLen++;

	Bin2Hex(pszProtocol, szProtocol, iApduLen);
	return 0;
}


void CHS_DemoDlg::OnBnClickedButtonOpenPort()
{
	// TODO:  在此添加控件通知处理程序代码



}
