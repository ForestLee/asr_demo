#ifndef _ASR_DEMO_H_
#define _ASR_DEMO_H_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CAsrDemoApp:
// See AsrDemo.cpp for the implementation of this class
//

class AsrDemo : public CWinApp
{
public:
	AsrDemo();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern AsrDemo theApp;

#endif //_ASR_DEMO_H_