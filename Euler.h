// Euler.h : main header file for the Euler application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CEulerApp:
// See Euler.cpp for the implementation of this class
//

class CEulerApp : public CWinApp
{
public:
	CEulerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CEulerApp theApp;