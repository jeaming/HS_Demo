
// InitialCard_HZ.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CInitialCard_HZApp:
// �йش����ʵ�֣������ InitialCard_HZ.cpp
//

class CInitialCard_HZApp : public CWinApp
{
public:
	CInitialCard_HZApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CInitialCard_HZApp theApp;