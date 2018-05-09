
// InitialCard_HZDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "PCSC_Operator.h"
#include "eyeoperator.h"
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>
#include <boost\property_tree\json_parser.hpp>

#include <TripleDES\TripleDES.h>

#define KEY_COUNT 14

#define ADMIN_CARD		0
#define SINGLE_CAR_CARD	1
#define AGENCY_CARD		2
#define PSAM_CARD		3

using namespace std;

// CInitialCard_HZDlg 对话框
class CInitialCard_HZDlg : public CDialogEx
{
// 构造
public:
	CInitialCard_HZDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_INITIALCARD_HZ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	typedef struct THREADSTRUCT			 	    //structure for passing to the controlling function
	{
      CInitialCard_HZDlg*	_this;
	}THREADSTRUCT;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnErasecard();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnCardtest();
	afx_msg void OnBnClickedBtnChangeUid();
	afx_msg void OnBnClickedBtnCheckUid();
	afx_msg void OnBnClickedBtnPurchase();
	afx_msg void OnBnClickedBtnRecordCsn();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	///////////////////////
	CButton m_BtnEraseCard;
	CButton m_BtnTestCard;
	CButton m_BtnStart;
	CButton m_BtnStop;
	CButton m_BtnChangeUID;
	CButton m_BtnGetUID;
	CButton m_BtnCardType;

	CEdit m_EdtDisp;
	CString m_StrDisp;
	CBrush m_brush;
	CEdit m_EdtSnBegin;
	CString m_strSnBegin;
	CComboBox m_cmbPersoType;

	CFont fontA, fontB;

public:
	// 发卡线程
	static UINT StartInitCardThread(LPVOID param);
	static UINT StartPurchase(LPVOID param);
	static UINT StartCheckCardConstruct(LPVOID param);
	CEyeOperator m_eyeOp;
	CPCSC_Operator m_pcscOp;
	TripleDES des;

	int DisplayHint(CString strDisp);

public:
	// Get current device set
	int GetInitSettings(void);
	int GetModulePath(char* pszFilePath);

	int UserCardReset(void);
	int PatchForCard(void);

	int InitialMasterFile(void);
	int ConstructCard(void);
	// 添加MifareKey
	int AppendMifareKey(void);


public:
	// 修改默认控制字
	int ChangeMifareControlByte(void);
	int SelectCard(void);
	// check control words
	int CheckControlWords(void);
	void BeepForSuccess(void);
	void BeepForFail(void);

	// 0值消费
	int Purchase(void);
	int LogUID(void);
	int IncrementSN(void);
	int GetLogDirectory(char* pszPath);


	int CheckUID(void);
	unsigned long RotateUID(const unsigned char* pszUID);

private:
	unsigned char m_szCardProperty[32];
	long lBegin;
	int m_iInitialErr;
	char m_szModulePath[MAX_PATH];
	char m_szFilePath[200];
	unsigned char m_szSN[40];
	unsigned char m_szCardKeys[KEY_COUNT][33];
	int m_iChangeUIDFlag;

	int m_isReaderOpen;
	int m_isDbOpen;
	int m_iDevIndex;
	int m_isNewDB;
	int m_iPatchLines;
	string strAPDU;
	string strPatch;
	string strChkConstructAPDU;
	string strRecoveryAPDU;
	string strCSNCommand;
	string strCSNType;
	string strPersoType;
	string strInterface;
	string strMifareInit;
	string m_strReplace;
	string m_strDeviceType;
	string m_strDeviceName;
	string m_strCity;
	string strPatch121129;

	CString strTemp;
	CString m_strUIDLog;

private:
	int m_iUID_Offset;
	char m_szDate[32];
	int m_iOutSend;
	int m_iCategory;
	unsigned char m_szCardUID[20];
	unsigned char m_szATR[128];
	BOOL m_bStarted;
	BOOL m_bStop;
	DWORD m_dwProtocol;
	unsigned char m_szDIV[20];
	unsigned char m_szCAK[33];
	unsigned char m_szTMK[33];
	unsigned char m_szDCCK[33];
	unsigned char m_szDCAK[17];
	unsigned char m_szDTMK[17];
	unsigned char m_szDCMK[17];
	unsigned char m_szATS[128];

	_ConnectionPtr m_pConn;
	_CommandPtr m_pSqlCommand;
	_RecordsetPtr pRecordset;

	CString m_strConn;
	VARIANT m_varNull;

public:
	int GetSN(unsigned char* pszUID_LSB, unsigned char* pszSN, int& iSNLength);
	int ICC_SendCommand(unsigned char* pszCommand, int iCmdLen, unsigned char* pszRetSW, unsigned char* pszRetData);
	int TripleEncrypt(unsigned char* pszKey, int iKeyLen, unsigned char* pszData, int iDataLen, unsigned char* pszRetData);
	int TripleDecrypt(unsigned char* pszKey, int iKeyLen, unsigned char* pszData, int iDataLen, unsigned char* pszRetData);
	int CheckCommand(string strCommand);
	int ExternalAuth_MK();
	int ExternalAuth_MK(unsigned char* pszKey, int iKeyLen);
	int DisplayHint(char* pszFormat, ...);
	int CreateAccessFile(char* pszFilePath);
	int ChangeMifareControlWord(void);//By SCM SDI010/011
	int EncryptData(unsigned char* pszData, int iLength, unsigned char* pszEncryptData);
	// 卡片检测
	int ConstructCheck(void);//in hex
	int RemoveDataPad(unsigned char* pszData, unsigned int* iDataLen);
	int DecryptData(string &strData);
	int m_bIsSelectedCard;//该标识作为是否对卡片进行操作的依据

	int ClearDisplay(void);
	int SetMifareAccInit(void);
	int ConnectReader();
	int InitDatabase();
	int SetWindFont();
	void SetPersoTypeCombo();
	int SetPersoAPDU(int iType);
	int DiversKeys();
	int InitializeMF();
	int WriteUserCardKeys();
	int WritePSAMCardKeys();
	afx_msg void OnCbnCloseupComboPersoType();
	afx_msg void OnCbnKillfocusComboPersoType();
};
