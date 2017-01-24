#include "stdafx.h"
#include "recognition.h"
#include "infoDlg.h"
#include "afxdialogex.h"
#include "FileFolderDialog.h"

#define WM_SELECT_CAMERA (WM_APP + 1)
#define WM_DECIDE_FOLDER (WM_APP + 2)

IMPLEMENT_DYNAMIC(CInfoDialog, CDialogEx)

CInfoDialog::CInfoDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CInfoDialog::IDD, pParent)
, m_name(_T(""))
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
	DDX_Text(pDX, IDC_EDT_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 16);
	DDX_Control(pDX, IDC_LST_IMAGE, m_listImg);
}

BEGIN_MESSAGE_MAP(CInfoDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_LST_CAMERA, &CInfoDialog::OnLbnSelchangeLstCamera)
	ON_BN_CLICKED(IDC_BTN_FOLDER, &CInfoDialog::OnBnClickedBtnFolder)
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

void CInfoDialog::DetectionCount(int count)
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

void CInfoDialog::RecognitionInfo(const wstring& name, const float& score)
{
	CWinApp *app = AfxGetApp();
	float iniscore = static_cast<float>(_ttof(app->GetProfileString(L"Threashold", L"score", L"0.8")));

	if (score > iniscore) {
		wchar_t strcount[32] = { 0 };
		_snwprintf_s<32>(strcount, _TRUNCATE, L"%.0f %%", (score * 100));
		m_count.SetWindowText(strcount);

		m_state.SetWindowText(name.c_str());
	}
}

void CInfoDialog::SetImageList(CString& item)
{
	m_listImg.AddString(item);
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

void CInfoDialog::OnBnClickedBtnFolder()
{
	//FileFolderDialog fol(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, _T("画像ファイル|*.jpg;*.jpeg;*.png||"), this);
	//if (fol.DoModal() == IDOK) {
	//	CString selfile = fol.GetFileName();
	//	if (selfile.GetLength() == 0) {
	//		CString selfol = fol.GetFolderPath();
	//		if (selfol.GetLength() != 0) {

	//		}
	//	}
	//}

	UpdateData(TRUE);

	if (m_name.GetLength() == 0) {
		MessageBox(_T("先に名前を入力してください"));
		return;
	}

	TCHAR cDirSel[256] = { 0 };
	if (SelectFolder(m_hWnd, _T(""), cDirSel, BIF_RETURNONLYFSDIRS, _T("顔画像のあるフォルダーを選択してください"))) {
		CWnd *par = AfxGetApp()->m_pMainWnd;
		par->SendMessage(WM_DECIDE_FOLDER, reinterpret_cast<WPARAM>(m_name.GetString()), reinterpret_cast<LPARAM>(cDirSel));
	}
}

int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
	case BFFM_INITIALIZED:
		//初期化処理(初期表示フォルダ指定)
		SendMessage(hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
		break;
	case BFFM_SELCHANGED:
		//フォルダ選択ダイアログでフォルダが選択された時に処理が必要であればここに記載
		TRACE(_T("フォルダーが選択されました\r\n"));
		break;
	}

	return 0;
}

BOOL CInfoDialog::SelectFolder(HWND hWnd, LPCTSTR lpDefFolder, LPTSTR lpSelectPath, UINT iFlg, CString ttl)
{
	LPMALLOC pMalloc;
	BOOL brtn = FALSE;

	if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
		BROWSEINFO brows;
		ITEMIDLIST *pIDlist;

		//BROWSEINFO構造体の初期化
		memset(&brows, NULL, sizeof(brows));

		//BROWSEINFO構造体に値をセット
		brows.hwndOwner = hWnd;
		brows.pszDisplayName = lpSelectPath;
		brows.lpszTitle = ttl;
		brows.lParam = (LPARAM)lpDefFolder;
		brows.pidlRoot = NULL;
		brows.ulFlags = iFlg;
		brows.lpfn = &BrowseCallbackProc;
		brows.iImage = (int)NULL;

		//フォルダ選択ダイアログの表示
		pIDlist = SHBrowseForFolder(&brows);

		if (pIDlist == NULL)
			//何も選択されなければデフォルトフォルダを返す
			wcscpy_s(lpSelectPath, 256, lpDefFolder);
		else {
			//ITEMIDLIST からフルパスを得る
			SHGetPathFromIDList(pIDlist, lpSelectPath);
			brtn = TRUE;

			//ITEMIDLIST の開放
			pMalloc->Free(pIDlist);
		}

		pMalloc->Release();
	}

	return brtn;
}
