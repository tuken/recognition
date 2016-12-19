#include "stdafx.h"
#include "detection.h"
#include "infoDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CInfoDialog, CDialogEx)

CInfoDialog::CInfoDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CInfoDialog::IDD, pParent)
{
	m_font1.CreatePointFont(340, L"メイリオ");
	m_font2.CreatePointFont(160, L"メイリオ");
}

CInfoDialog::~CInfoDialog()
{
	m_font1.DeleteObject();
	m_font2.DeleteObject();
}

void CInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_STATE, m_state);
	DDX_Control(pDX, IDC_STC_COUNT, m_count);
	DDX_Control(pDX, IDC_STC_CAMERA, m_camera);
}

BEGIN_MESSAGE_MAP(CInfoDialog, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CInfoDialog::OnCancel()
{
	DestroyWindow();
}

//void CInfoDialog::PostNcDestroy()
//{
//	delete this;
//}

BOOL CInfoDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_state.SetFont(&m_font1);
	m_count.SetFont(&m_font2);

	return TRUE;
}

HBRUSH CInfoDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
		if (*pWnd == m_state)
			pDC->SetTextColor(RGB(20, 230, 20));

	return hbr;
}

void CInfoDialog::DetecttionCount(int count)
{
	wchar_t strcount[32] = { 0 };
	_snwprintf_s<32>(strcount, _TRUNCATE, L"%d 人", count);
	m_count.SetWindowText(strcount);
}

void CInfoDialog::SetCameraName(const wchar_t *name)
{
	if (name != NULL)
		m_camera.SetWindowText(name);
}
