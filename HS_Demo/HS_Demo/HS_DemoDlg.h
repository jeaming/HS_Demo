
// HS_DemoDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CHS_DemoDlg 对话框
class CHS_DemoDlg : public CDialogEx
{
// 构造
public:
	CHS_DemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HS_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtAPDU;
	CEdit m_edtProtocol;
	CComboBox m_cmbCardSeat;
	CButton m_btnResetProtocol;
	afx_msg void OnBnClickedButtonRst();
	afx_msg void OnBnClickedButtonGenProtocol();
	// Apdu
	int ConstructProtocol(CString strAPDU, int iCardSeat, unsigned char* pszProtocol);
	CComboBox m_cmbPort;
	CEdit m_edtAdminCardN;
	CEdit m_edtCardNO;
	CComboBox m_cmbFileID;
	CEdit m_edtDisp;
	afx_msg void OnBnClickedButtonOpenPort();
};
