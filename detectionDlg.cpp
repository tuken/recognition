#include "stdafx.h"
#include "detection.h"
#include "detectionDlg.h"
#include "afxdialogex.h"
#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDetectionDlg::CDetectionDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CDetectionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDetectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIDEO, _video);
}

BEGIN_MESSAGE_MAP(CDetectionDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

BOOL CDetectionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	IBaseFilter *pSampleGrabberFilter;
	ISampleGrabber *pSampleGrabber;
	AM_MEDIA_TYPE am_media_type;

	// SampleGrabber(Filter)を生成
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID *)&pSampleGrabberFilter);

	// FilterからISampleGrabberインターフェースを取得します
	pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID *)&pSampleGrabber);

	// SampleGrabberを接続するフォーマットを指定。
	// ここがポイントです。
	// ここの指定の仕方によりSampleGrabberの挿入箇所を
	// 決定できます。このサンプルのような指定をすると
	// 画面出力の寸前でサンプルを取得できます。
	ZeroMemory(&am_media_type, sizeof(am_media_type));
	am_media_type.majortype = MEDIATYPE_Video;
	am_media_type.subtype = MEDIASUBTYPE_RGB24;
	am_media_type.formattype = FORMAT_VideoInfo;
	pSampleGrabber->SetMediaType(&am_media_type);

	IGraphBuilder *pGraphBuilder = NULL;
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&pGraphBuilder);

	// GraphにSampleGrabber Filterを追加
	pGraphBuilder->AddFilter(pSampleGrabberFilter, L"Sample Grabber");

	// CaptureGraphBuilder2というキャプチャ用GraphBuilderを生成する
	ICaptureGraphBuilder2 *pCaptureGraphBuilder2 = NULL;
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (LPVOID *)&pCaptureGraphBuilder2);

	// FilterGraphをセットする
	pCaptureGraphBuilder2->SetFiltergraph(pGraphBuilder);

	// MediaControlインターフェース取得
	IMediaControl *pMediaControl = NULL;
	pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&pMediaControl);

	// デバイスを列挙するためのCreateDevEnumを生成
	ICreateDevEnum *pCreateDevEnum = NULL;
	CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (PVOID *)&pCreateDevEnum);

	// VideoInputDeviceを列挙するためのEnumMonikerを生成 
	IEnumMoniker *pEnumMoniker = NULL;
	pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnumMoniker, 0);
	if (pEnumMoniker == NULL) {
		// 接続された映像入力デバイスが一つも無い場合にはこのif文に入ります
		printf("no device\n");
		return 0;
	}

	// EnumMonikerをResetする
	// Resetすると、先頭から数えなおします
	pEnumMoniker->Reset();

	// 最初のMonikerを取得
	IMoniker *pMoniker = NULL;
	ULONG nFetched = 0;
	pEnumMoniker->Next(1, &pMoniker, &nFetched);

	// MonkierをFilterにBindする
	IBaseFilter *pDeviceFilter = NULL;
	pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&pDeviceFilter);

	// FilterGraphにデバイスフィルタを追加する
	pGraphBuilder->AddFilter(pDeviceFilter, L"Device Filter");

	// この時点でMonkier系、Enumerator系は用済み
	pMoniker->Release();
	pEnumMoniker->Release();
	pCreateDevEnum->Release();

	// Graphを生成する
	hr = pCaptureGraphBuilder2->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pDeviceFilter, pSampleGrabberFilter, NULL);
	pSampleGrabber->GetConnectedMediaType(&am_media_type);
	pSampleGrabber->SetBufferSamples(TRUE);
	pSampleGrabber->SetCallback(this, 1);

	IVideoWindow *pVideoWin = NULL;
	hr = pGraphBuilder->QueryInterface(IID_IVideoWindow, (void **)&pVideoWin);

	pVideoWin->put_Owner((OAHWND)_video.m_hWnd);
	pVideoWin->put_WindowStyle(WS_CHILD);

	//GetClientRect(ghwndApp, &rc);
	//cyBorder = GetSystemMetrics(SM_CYBORDER);
	//cy = 150 + cyBorder;
	//rc.bottom -= cy;

	pVideoWin->SetWindowPosition(0, 0, 400, 300);
	pVideoWin->put_Visible(-1);

	// 再生開始
	pMediaControl->Run();

	//// 資源を解放
	//pMediaControl->Release();
	//pCaptureGraphBuilder2->Release();
	//pGraphBuilder->Release();

	//// COM終了
	//CoUninitialize();

	//return 0;




	////m_ctrlListDevices.ResetContent();
	////gcap.iNumVCapDevices = CountAllDevices();
	//int count = CountAllDevices();

	////int ii = 0;
	////while (ii < count) m_ctrlListDevices.AddString(strInfo[ii++]);

	////ghwndApp = this->m_hWnd;
	////gcap.FrameRate = 15.0;
	////gcap.pmVideo = 0;

	//TCHAR szVideoDisplayName[1024];
	//ZeroMemory(szVideoDisplayName, sizeof(szVideoDisplayName));
	//GetProfileString(TEXT("MyWEBCAM"), TEXT("VideoDevice2"), TEXT(""), szVideoDisplayName, NUMELMS(szVideoDisplayName));

	//WCHAR wszVideo[1024];
	//ZeroMemory(wszVideo, sizeof(wszVideo));
	//int ii = 0;
	//while (szVideoDisplayName[ii] != 0) {
	//	wszVideo[ii] = szVideoDisplayName[ii];
	//	ii++;
	//}

	//IBindCtx *lpBC = 0;
	//IMoniker *pmVideo = 0;

	//HRESULT hr = CreateBindCtx(0, &lpBC);
	//if (SUCCEEDED(hr)) {
	//	DWORD dwEaten;
	//	hr = MkParseDisplayName(lpBC, wszVideo, &dwEaten, &pmVideo);
	//	lpBC->Release();
	//}

	//StartThisDevice(pmVideo);

	////dlgWin2.Create(IDD_WIN2, this);
	////dlgWin2.ShowWindow(SW_SHOW);

	//IMonRelease(pmVideo);

	//SetTimer(1, 40, 0);

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
	wchar_t msg[128] = { 0 };
	_snwprintf_s<128>(msg, _TRUNCATE, L"BufferCB SampleTime[%.4f] BufferLen[%d]\r\n", SampleTime, BufferLen);
	OutputDebugString(msg);
	return S_OK;
}

void IMonRelease(IMoniker *&pm)
{
	if (pm) {
		pm->Release();
		pm = 0;
	}
}

//int CDetectionDlg::CountAllDevices()
//{
//	UINT uIndex = 0;
//	HRESULT hr;
//
//	IMoniker *rgpmVideoMenu[5];
//	for (int i = 0; i < NUMELMS(rgpmVideoMenu); i++)
//		IMonRelease(rgpmVideoMenu[i]);
//
//	ICreateDevEnum *pDevEnum = 0;
//	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pDevEnum);
//	if (hr != NOERROR) {
//		//strInfo[iIndex++].Format(TEXT("Error Creating Device Enumerator"));
//		return -1;
//	}
//
//	IEnumMoniker *pEnum = 0;
//	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
//	if (hr != NOERROR) {
//		//strInfo[iIndex++].Format(TEXT("!!!  No webcam found !!!"));
//		goto ENUM_EXIT;
//	}
//
//	pEnum->Reset();
//	ULONG uFetched;
//	IMoniker *pM;
//
//	while (1) {
//		hr = pEnum->Next(1, &pM, &uFetched);
//		if (hr != S_OK) goto ENUM_EXIT;
//
//		IPropertyBag *pBag = 0;
//
//		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
//		if (SUCCEEDED(hr)) {
//			VARIANT var;
//			var.vt = VT_BSTR;
//			hr = pBag->Read(L"FriendlyName", &var, NULL);
//			if (hr == NOERROR) {
//				//strInfo[uIndex].Format(L"%S", var.bstrVal);
//				SysFreeString(var.bstrVal);
//
//				ASSERT(rgpmVideoMenu[uIndex] == 0);
//				rgpmVideoMenu[uIndex] = pM;
//				pM->AddRef();
//			}
//
//			pBag->Release();
//		}
//
//		pM->Release();
//		uIndex++;
//	}
//
//	pEnum->Release();
//
//ENUM_EXIT:
//	pDevEnum->Release();
//	return uIndex;
//}
//
//void CDetectionDlg::StartThisDevice(IMoniker *pmVideo)
//{
//	if (pmVideo != pmVideo) {
//		if (pmVideo)
//			pmVideo->AddRef();
//
//		IMonRelease(gcap.pmVideo);
//
//		gcap.pmVideo = pmVideo;
//
//		if (gcap.fPreviewing) StopPreview();
//
//		if (gcap.fCaptureGraphBuilt || gcap.fPreviewGraphBuilt)
//			TearDownGraph();
//
//		FreeCapFilters();
//		if (InitCapFilters() == false) return;
//
//		BuildPreviewGraph();
//		StartPreview();
//	}
//}
//
//BOOL CDetectionDlg::BuildPreviewGraph()
//{
//	int cy, cyBorder;
//	HRESULT hr;
//
//	hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, gcap.pVCap, NULL, NULL);
//	if (hr == VFW_S_NOPREVIEWPIN)
//		gcap.fPreviewFaked = TRUE;
//	else if (hr != S_OK) {
//		strInfo[iIndex++].Format(TEXT("!!! This graph cannot preview !!!"));
//		gcap.fPreviewGraphBuilt = FALSE;
//		return FALSE;
//	}
//
//	hr = gcap.pFg->QueryInterface(IID_IVideoWindow, (void **)&gcap.pVW);
//	if (hr != NOERROR)
//		strInfo[iIndex++].Format(TEXT("!!! This graph cannot preview properly !!!"));
//	else {
//		RECT rc;
//		gcap.pVW->put_Owner((OAHWND)ghwndApp);    // We own the window now
//		gcap.pVW->put_WindowStyle(WS_CHILD);      // you are now a child
//
//		GetClientRect(ghwndApp, &rc);
//		cyBorder = GetSystemMetrics(SM_CYBORDER);
//		cy = 150 + cyBorder;
//		rc.bottom -= cy;
//
//		gcap.pVW->SetWindowPosition(0, 0, ImageX, ImageY);
//		gcap.pVW->put_Visible(-1);
//	}
//
//	gcap.fPreviewGraphBuilt = TRUE;
//	return TRUE;
//}
//
//BOOL CDetectionDlg::StartPreview()
//{
//	//if (gcap.fPreviewing) return TRUE;
//
//	//if (!gcap.fPreviewGraphBuilt) return FALSE;
//
//	IMediaControl *pMC = NULL;
//	HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **) &pMC);
//	if (SUCCEEDED(hr)) {
//		hr = pMC->Run();
//		if (FAILED(hr))
//			pMC->Stop();
//
//		pMC->Release();
//	}
//
//	if (FAILED(hr)) {
//		//strInfo[iIndex++].Format(TEXT("Error %x: Cannot run preview graph"), hr);
//		return FALSE;
//	}
//
//	//gcap.fPreviewing = TRUE;
//	return TRUE;
//}
//
//BOOL CDetectionDlg::StopPreview()
//{
//	//if (!gcap.fPreviewing) return FALSE;
//
//	IMediaControl *pMC = NULL;
//	HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **) &pMC);
//	if (SUCCEEDED(hr)) {
//		hr = pMC->Stop();
//		pMC->Release();
//	}
//
//	if (FAILED(hr)) {
//		//strInfo[iIndex++].Format(TEXT("Error %x: Cannot stop preview graph"), hr);
//		return FALSE;
//	}
//
//	//gcap.fPreviewing = FALSE;
//	return TRUE;
//}
