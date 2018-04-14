
// HS_DemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CHS_DemoDlg �Ի���
class CHS_DemoDlg : public CDialogEx
{
// ����
public:
	CHS_DemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HS_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
