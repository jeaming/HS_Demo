
// HS_Demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHS_DemoApp: 
// �йش����ʵ�֣������ HS_Demo.cpp
//

class CHS_DemoApp : public CWinApp
{
public:
	CHS_DemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHS_DemoApp theApp;