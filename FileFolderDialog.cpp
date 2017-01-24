#include "stdafx.h"
#include "FileFolderDialog.h"

FileFolderDialog::FileFolderDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, CWnd *pParentWnd)
: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_oldFolder = _T("");
}

FileFolderDialog::~FileFolderDialog()
{
}

void FileFolderDialog::OnFolderChange()
{
	CString strFolderPath = GetFolderPath();

	if (m_oldFolder != strFolderPath)
		m_oldFolder = strFolderPath;
	else {
		CPoint point;
		GetCursorPos(&point);
		CRect rect;
		GetParent()->GetDlgItem(IDOK)->GetWindowRect(&rect);
		// マウスポインタが「開く」ボタンの上にある
		if (point.x >= rect.left && point.x <= rect.right && point.y >= rect.top && point.y <= rect.bottom) {
			// フォルダ名をセット
			wcsncpy_s(m_ofn.lpstrFile, m_ofn.nMaxFile, (LPCWSTR)strFolderPath, _TRUNCATE);
			if (OnFileNameOK() == FALSE) {
				// ダイアログを閉じる
				CDialog *pParent = (CDialog *)GetParent();
				if (pParent != NULL)
					pParent->EndDialog(IDOK);
			}
		}
	}

	CFileDialog::OnFolderChange();
}
