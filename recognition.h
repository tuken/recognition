#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"

class CDetectionApp : public CWinApp
{
public:
	CDetectionApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CDetectionApp theApp;
