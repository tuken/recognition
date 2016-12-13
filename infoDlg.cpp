#include "stdafx.h"
#include "detection.h"
#include "infoDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CInfoDialog, CDialogEx)

CInfoDialog::CInfoDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CInfoDialog::IDD, pParent)
{
}

CInfoDialog::~CInfoDialog()
{
}

void CInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_STATE, m_state);
	DDX_Control(pDX, IDC_STC_COUNT, m_count);
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

	CFont f1;
	f1.CreatePointFont(340, L"メイリオ");
	m_state.SetFont(&f1);
	//f1.DeleteObject();
	CFont f2;
	f2.CreatePointFont(160, L"メイリオ");
	m_count.SetFont(&f2);
	f2.DeleteObject();

	return TRUE;
}


HBRUSH CInfoDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (nCtlColor == CTLCOLOR_STATIC)
		if (*pWnd == m_state)
			pDC->SetTextColor(RGB(20, 250, 20));

	return hbr;
}

void CInfoDialog::DetecttionCount(int count)
{
	wchar_t strcount[32] = { 0 };
	_snwprintf_s<32>(strcount, _TRUNCATE, L"%d 人", count);
	m_count.SetWindowText(strcount);
}
