#include "stdafx.h"
#include "detection.h"
#include "infoDlg.h"
#include "afxdialogex.h"

#define WM_SELECT_CAMERA (WM_APP + 1)

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
	DDX_Control(pDX, IDC_LST_CAMERA, m_listCam);
}

BEGIN_MESSAGE_MAP(CInfoDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_LST_CAMERA, &CInfoDialog::OnLbnSelchangeLstCamera)
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

	CWinApp *app = AfxGetApp();
	UINT empty = app->GetProfileInt(L"Threashold", L"empty", 2);
	UINT crowd = app->GetProfileInt(L"Threashold", L"crowd", 5);

	if (static_cast<UINT>(count) <= empty)
		m_state.SetWindowText(L"空");
	else if (static_cast<UINT>(count) >= crowd)
		m_state.SetWindowText(L"混雑");
	else
		m_state.SetWindowText(L"やや混雑");
}

//void CInfoDialog::SetCameraName(const wchar_t *name)
//{
//	if (name != NULL)
//		m_camera.SetWindowText(name);
//}

void CInfoDialog::SetCameraList(CComPtr<IEnumMoniker>& mons)
{
	// EnumMonikerをResetする Resetすると、先頭から数えなおします
	HRESULT hr = mons->Reset();

	// 最初のMonikerを取得
	CComPtr<IMoniker> moniker;
	ULONG nFetched = 0;
	while ((hr = mons->Next(1, &moniker, &nFetched)) == S_OK) {
		CComPtr<IPropertyBag> prop;
		hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&prop);
		if (SUCCEEDED(hr)) {
			VARIANT varName;
			VariantInit(&varName);
			hr = prop->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr))
				m_listCam.AddString(varName.bstrVal);

			VariantClear(&varName);
			moniker.Release();
		}
	}
}

void CInfoDialog::OnLbnSelchangeLstCamera()
{
	int sel = m_listCam.GetCurSel();
	if (sel != LB_ERR) {
		m_listCam.EnableWindow(FALSE);
		CString text;
		m_listCam.GetText(sel, text);
		CWnd *par = AfxGetApp()->m_pMainWnd;
		par->SendMessage(WM_SELECT_CAMERA, 0, reinterpret_cast<LPARAM>(text.AllocSysString()));
	}
}
