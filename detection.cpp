#include "stdafx.h"
#include "detection.h"
#include "detectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CDetectionApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CDetectionApp::CDetectionApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CDetectionApp theApp;

BOOL CDetectionApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	//SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	if (m_pszProfileName) {
		TCHAR szPath[MAX_PATH];
		::GetModuleFileName(NULL, szPath, MAX_PATH);  // 実行ファイルのパスを取得
		::PathRenameExtension(szPath, _T(".ini"));  // 拡張子をiniへ変更
		free((void *)m_pszProfileName);  // メモリの解放
		m_pszProfileName = _tcsdup(szPath);  // 新しいパスを設定 
	}

	CDetectionDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
	}
	else if (nResponse == IDCANCEL) {
	}
	else if (nResponse == -1) {
		TRACE(traceAppMsg, 0, "警告: ダイアログの作成に失敗しました。アプリケーションは予期せずに終了します。\n");
	}

	return FALSE;
}

