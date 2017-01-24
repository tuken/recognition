#pragma once

#include "afxdlgs.h"

class FileFolderDialog : public CFileDialog
{
public:
	FileFolderDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd *pParentWnd);
	~FileFolderDialog();

private:
	CString m_oldFolder;
	virtual void OnFolderChange();
};

