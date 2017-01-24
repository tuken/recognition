#include "stdafx.h"
#include "recognition.h"
#include "recognitionDlg.h"
#include "afxdialogex.h"
#include <initguid.h>

#include <AyonixFaceID.h>

#include <algorithm>
#include <functional>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_IDEL (WM_USER + 1)
#define WM_SELECT_CAMERA (WM_APP + 1)
#define WM_DECIDE_FOLDER (WM_APP + 2)

CDetectionDlg::CDetectionDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CDetectionDlg::IDD, pParent)
, m_once(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	hr = m_gb.CoCreateInstance(CLSID_FilterGraph);

	memset(&m_mt, 0, sizeof(m_mt));

	m_dlg.Create(IDD_INFO_DIALOG, this);
}

void CDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIDEO, m_video);
}

BEGIN_MESSAGE_MAP(CDetectionDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_SELECT_CAMERA, OnSelectCamera)
	ON_MESSAGE(WM_DECIDE_FOLDER, OnDecideFolder)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

BOOL CDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//int res = AFIDInitialize(TEXT("C:\\Program Files (x86)\\Ayonix\\FaceID\\data\\engine"), &m_engine);
	int res = AFIDInitialize(TEXT("data\\engine"), &m_engine);
	if (res != AYNX_OK) {
		wchar_t msg[128] = { 0 };
		_snwprintf_s<128>(msg, _TRUNCATE, L"顔認識ライブラリの初期化に失敗しました [%d]\r\n", res);
		MessageBox(msg);
		//return TRUE;
	}

	// デバイスを列挙するためのCreateDevEnumを生成
	CComPtr<ICreateDevEnum> devs;
	HRESULT hr = devs.CoCreateInstance(CLSID_SystemDeviceEnum);

	// VideoInputDeviceを列挙するためのEnumMonikerを生成 
	hr = devs->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &m_mons, 0);
	if (FAILED(hr) || !m_mons) {
		// 接続された映像入力デバイスが一つも無い場合にはこのif文に入ります
		MessageBox(L"カメラが見つかりません");
		return TRUE;
	}

	m_dlg.SetCameraList(m_mons);

	return TRUE;
}

void CDetectionDlg::OnPaint()
{
	if (IsIconic())	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CDialogEx::OnPaint();
}

HCURSOR CDetectionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

STDMETHODIMP CDetectionDlg::SampleCB(double SampleTime, IMediaSample *pSample)
{
	wchar_t msg[128] = { 0 };
	_snwprintf_s<128>(msg, _TRUNCATE, L"SampleCB SampleTime[%.4f]\r\n", SampleTime);
	OutputDebugString(msg);
	return S_OK;
}

STDMETHODIMP CDetectionDlg::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{

	BITMAPFILEHEADER bfh = { 0 };
	VIDEOINFOHEADER *vif = reinterpret_cast<VIDEOINFOHEADER *>(m_mt.pbFormat);

	bfh.bfType = ('M' << 8) | 'B';
	bfh.bfSize = sizeof(bfh) + sizeof(BITMAPINFOHEADER) + BufferLen;
	bfh.bfOffBits = sizeof(bfh) + sizeof(BITMAPINFOHEADER);

	BYTE *buf = static_cast<BYTE *>(malloc(sizeof(bfh) + sizeof(BITMAPINFOHEADER) + BufferLen));
	memcpy(buf, &bfh, sizeof(bfh));
	memcpy(buf + sizeof(bfh), &vif->bmiHeader, sizeof(BITMAPINFOHEADER));
	memcpy(buf + sizeof(bfh) + sizeof(BITMAPINFOHEADER), pBuffer, BufferLen);

	//detect(buf, sizeof(bfh) + sizeof(BITMAPINFOHEADER) + BufferLen);
	recognize(buf, sizeof(bfh) + sizeof(BITMAPINFOHEADER) + BufferLen);

	free(buf);

	return S_OK;
}

void CDetectionDlg::detect(BYTE *buf, int len)
{
	//wchar_t msg[128] = { 0 };
	AynxImage *img = NULL;
	int res = AFIDDecodeImage(buf, len, &img);
	//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDDecodeImage res[%d]\r\n", res);
	//OutputDebugString(msg);
	if (res == AYNX_OK) {
		AynxFace *faces = NULL;
		size_t count = 0;
		res = AFIDDetectFaces(m_engine, img, &faces, &count, NULL);
		//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDDetectFaces res[%d]\r\n", res);
		//OutputDebugString(msg);
		if (res == AYNX_OK) {
			WINDOWINFO wi = { 0 };
			if (m_dlg.GetWindowInfo(&wi))
				m_dlg.DetectionCount(count);
			//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDDetectFaces count[%d]\r\n", count);
			//OutputDebugString(msg);

			AFIDReleaseFaces(faces, count);
		}

		AFIDReleaseImage(img);
	}

	//DWORD nWritten;
	//HANDLE h = CreateFile(L"result.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//WriteFile(h, &bfh, sizeof(bfh), &nWritten, NULL);
	//WriteFile(h, &vif->bmiHeader, sizeof(BITMAPINFOHEADER), &nWritten, NULL);
	//WriteFile(h, pBuffer, BufferLen, &nWritten, NULL);
	//CloseHandle(h);
}

void CDetectionDlg::recognize(BYTE *buf, int len)
{
	if (m_afids.empty()) return;

	//wchar_t msg[128] = { 0 };
	AynxImage *img = NULL;
	int res = AFIDDecodeImage(buf, len, &img);
	//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDDecodeImage res[%d]\r\n", res);
	//OutputDebugString(msg);
	if (res == AYNX_OK) {
		AynxFace *faces = NULL;
		size_t nFaces = 0;
		res = AFIDDetectFaces(m_engine, img, &faces, &nFaces, 0);
		//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDDetectFaces res[%d] count[%d]\r\n", res, nFaces);
		//OutputDebugString(msg);
		if (res == AYNX_OK) {
			if (nFaces > 0) {
				res = AFIDPreprocessFace(m_engine, faces);
				//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDPreprocessFace res[%d]\r\n", res);
				//OutputDebugString(msg);
				if (res == AYNX_OK) {
					if (faces[0].isValid != 0) {
						void *afid = 0;
						size_t afidSize = 0;
						res = AFIDCreateAfid(m_engine, faces, &afid, &afidSize);
						//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDCreateAfid res[%d] afidSize[%d]\r\n", res, afidSize);
						//OutputDebugString(msg);
						if (res == AYNX_OK) {
							vector<float> scores(m_afids.size(), 0);
							vector<int> indexes(m_afids.size(), 0);
							res = AFIDMatchAfids(m_engine, afid, &(m_afids[0]), m_afids.size(), &(scores[0]), &(indexes[0]));
							//_snwprintf_s<128>(msg, _TRUNCATE, L"AFIDMatchAfids res[%d]\r\n", res);
							//OutputDebugString(msg);
							if (res == AYNX_OK) {
								vector<int>::iterator ii = indexes.begin();
								for (vector<float>::iterator iter = scores.begin(); iter != scores.end(); iter++, ii++) {
									//_snwprintf_s<128>(msg, _TRUNCATE, L"\t score[%.2f]\r\n", *iter);
									//OutputDebugString(msg);
									if (*iter > .80f)
										m_dlg.RecognitionInfo(m_names[*ii], *iter);
								}
							}

							AFIDReleaseAfid(afid);
						}
					}
				}
			}

			AFIDReleaseFaces(faces, nFaces);
		}

		AFIDReleaseImage(img);
	}
}

void CDetectionDlg::OnClose()
{
	m_dlg.DestroyWindow();

	CComQIPtr<IMediaControl> ctrl = m_gb;
	ctrl->Stop();

	AFIDFinalize(m_engine);

	__super::OnClose();
}

void CDetectionDlg::OnIdle()
{
	CRect rect;
	GetWindowRect(&rect);
	//GetClientRect(&rect);
	//ClientToScreen(&rect);
	//ScreenToClient(&rect);
	m_dlg.ShowWindow(SW_SHOW);
	m_dlg.SetWindowPos(this, rect.right, rect.top, 0, 0, SWP_NOSIZE);
}

LRESULT CDetectionDlg::OnSelectCamera(WPARAM wParam, LPARAM lParam)
{
	CComBSTR camname;
	camname.Attach(reinterpret_cast<BSTR>(lParam));

	// SampleGrabber(Filter)を生成
	CComPtr<ISampleGrabber> grabber;
	HRESULT hr = grabber.CoCreateInstance(CLSID_SampleGrabber);

	// FilterからISampleGrabberインターフェースを取得します
	CComQIPtr<IBaseFilter> grbFilter = grabber;

	// SampleGrabberを接続するフォーマットを指定。
	// ここがポイントです。
	// ここの指定の仕方によりSampleGrabberの挿入箇所を決定できます。このサンプルのような指定をすると
	// 画面出力の寸前でサンプルを取得できます。
	m_mt.majortype = MEDIATYPE_Video;
	m_mt.subtype = MEDIASUBTYPE_RGB24;
	m_mt.formattype = FORMAT_VideoInfo;
	hr = grabber->SetMediaType(&m_mt);

	// GraphにSampleGrabber Filterを追加
	hr = m_gb->AddFilter(grbFilter, L"Sample Grabber");

	// CaptureGraphBuilder2というキャプチャ用GraphBuilderを生成する
	CComPtr<ICaptureGraphBuilder2> capgb;
	hr = capgb.CoCreateInstance(CLSID_CaptureGraphBuilder2);

	// FilterGraphをセットする
	capgb->SetFiltergraph(m_gb);

	// MediaControlインターフェース取得
	CComQIPtr<IMediaControl> ctrl = m_gb;

	// EnumMonikerをResetする Resetすると、先頭から数えなおします
	hr = m_mons->Reset();

	// 最初のMonikerを取得
	CComPtr<IMoniker> moniker;
	ULONG nFetched = 0;
	while ((hr = m_mons->Next(1, &moniker, &nFetched)) == S_OK) {
		CComPtr<IPropertyBag> prop;
		hr = moniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&prop);
		if (SUCCEEDED(hr)) {
			VARIANT varName;
			VariantInit(&varName);
			hr = prop->Read(L"FriendlyName", &varName, 0);
			if (camname == varName.bstrVal) {
				VariantClear(&varName);
				break;
			}

			VariantClear(&varName);
			moniker.Release();
		}
	}

	// MonkierをFilterにBindする
	CComPtr<IBaseFilter> devFilter;
	hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&devFilter);

	// FilterGraphにデバイスフィルタを追加する
	hr = m_gb->AddFilter(devFilter, L"Device Filter");

	CComPtr<IBaseFilter> rndFilter;
	hr = rndFilter.CoCreateInstance(CLSID_VideoRenderer);

	hr = m_gb->AddFilter(rndFilter, L"Video Renderer");

	// Graphを生成する
	hr = capgb->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, devFilter, grbFilter, rndFilter);
	hr = grabber->GetConnectedMediaType(&m_mt);
	hr = grabber->SetBufferSamples(TRUE);
	hr = grabber->SetCallback(this, 1);

	CComQIPtr<IVideoWindow> win = rndFilter;
	hr = win->put_Owner((OAHWND)m_video.m_hWnd);
	hr = win->put_WindowStyle(WS_CHILD);

	CRect rc;
	m_video.GetClientRect(&rc);
	//cyBorder = GetSystemMetrics(SM_CYBORDER);
	//cy = 150 + cyBorder;
	//rc.bottom -= cy;

	hr = win->SetWindowPosition(0, 0, rc.Width(), rc.Height());
	hr = win->put_Visible(-1);

	// 再生開始
	ctrl->Run();

	return 0;
}

LRESULT CDetectionDlg::OnDecideFolder(WPARAM wParam, LPARAM lParam)
{
	wchar_t *name = reinterpret_cast<wchar_t *>(wParam);
	wchar_t *folder = reinterpret_cast<wchar_t *>(lParam);

	wpath p(folder);
	vector<wstring> failfiles;
	for (wdirectory_iterator iter(p); iter != wdirectory_iterator(); iter++) {
		if (is_regular_file((*iter).path())) {
			wstring ext = (*iter).path().extension();
			if (ext == _T(".bmp") || ext == _T(".png") || ext == _T(".gif") || ext == _T(".jpg") || ext == _T(".jp2") || ext == _T(".tiff")) {
				AynxImage *img = NULL;
				int ret = AFIDLoadImage((*iter).path().string().c_str(), &img);
				if (ret != AYNX_OK) {
					failfiles.push_back((*iter).path());
				}

				AynxFace *faces = NULL;
				size_t nFaces = 0;
				ret = AFIDDetectFaces(m_engine, img, &faces, &nFaces, 0);
				if (ret != AYNX_OK) {
					AFIDReleaseImage(img);
					failfiles.push_back((*iter).path());
					continue;
				}

				if (nFaces == 0) {
					AFIDReleaseImage(img);
					failfiles.push_back((*iter).path());
					continue;
				}

				ret = AFIDPreprocessFace(m_engine, faces);
				if (ret != AYNX_OK) {
					AFIDReleaseFaces(faces, nFaces);
					AFIDReleaseImage(img);
					failfiles.push_back((*iter).path());
					continue;
				}

				if (faces[0].isValid == 0) {
					AFIDReleaseFaces(faces, nFaces);
					AFIDReleaseImage(img);
					failfiles.push_back((*iter).path());
					continue;
				}

				void *afid = 0;
				size_t afidSize = 0;
				ret = AFIDCreateAfid(m_engine, faces, &afid, &afidSize);
				if (ret != AYNX_OK) {
					AFIDReleaseFaces(faces, nFaces);
					AFIDReleaseImage(img);
					failfiles.push_back((*iter).path());
					continue;
				}

				AFIDReleaseFaces(faces, nFaces);
				AFIDReleaseImage(img);

				m_names.push_back(name);
				m_afids.push_back(afid);
			}
		}
		//else if (is_directory) {}
	}

	if (!failfiles.empty()) {
		wchar_t msg[2048] = { 0 };
		wcscpy_s<2048>(msg, L"以下の画像は読み込めませんでした\r\n");
		for (vector<wstring>::iterator i = failfiles.begin(); i != failfiles.end(); i++) {
			wcscat_s<2048>(msg, i->c_str());
			wcscat_s<2048>(msg, L"\r\n");
		}

		MessageBox(msg);
	}

	if (m_afids.empty())
		MessageBox(_T("選択したフォルダーには画像はありません"));
	else {
		CString item;
		item.Format(_T("%s (%d枚)"), name, m_afids.size());
		m_dlg.SetImageList(item);
	}

	return 0;
}

LRESULT CDetectionDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_IDEL)
		OnIdle();

	return __super::WindowProc(message, wParam, lParam);
}

void CDetectionDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);

	if (bShow && !m_once) {
		m_once = true;
		PostMessage(WM_IDEL, MAKEWPARAM(0, 0), NULL);
	}
}

void CDetectionDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	wchar_t msg[128] = { 0 };
	_snwprintf_s<128>(msg, _TRUNCATE, L"OnActivate nState[%d] pWndOther[0x%08x] bMinimized[%s]\r\n", nState, pWndOther, (bMinimized ? L"TRUE": L"FALSE"));
	OutputDebugString(msg);
	if (pWndOther == this || pWndOther == NULL) {
		if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE) {
			OutputDebugString(L"OnActivate SetWindowPos\r\n");
			m_dlg.SetWindowPos(this, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

	__super::OnActivate(nState, pWndOther, bMinimized);
}
