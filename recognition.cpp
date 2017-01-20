#include "stdafx.h"
#include "recognition.h"
#include "recognitionDlg.h"

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

	//SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	if (m_pszProfileName) {
		TCHAR szPath[MAX_PATH];
		::GetModuleFileName(NULL, szPath, MAX_PATH);  // ���s�t�@�C���̃p�X���擾
		::PathRenameExtension(szPath, _T(".ini"));  // �g���q��ini�֕ύX
		free((void *)m_pszProfileName);  // �������̉��
		m_pszProfileName = _tcsdup(szPath);  // �V�����p�X��ݒ� 
	}

	CDetectionDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK) {
	}
	else if (nResponse == IDCANCEL) {
	}
	else if (nResponse == -1) {
		TRACE(traceAppMsg, 0, "�x��: �_�C�A���O�̍쐬�Ɏ��s���܂����B�A�v���P�[�V�����͗\�������ɏI�����܂��B\n");
	}

	return FALSE;
}

