#pragma once
#include "afxwin.h"
#include "qedit.h"
#include <atlbase.h>

class CDetectionDlg : public CDialogEx, ISampleGrabberCB
{
public:
	CDetectionDlg(CWnd *pParent = NULL);

	enum { IDD = IDD_DETECTION_DIALOG };

	STDMETHODIMP_(ULONG) CDetectionDlg::AddRef()
	{
		return ++m_dwRef;
	}

	STDMETHODIMP_(ULONG) CDetectionDlg::Release()
	{
		if (--m_dwRef == 0) {
			delete this;
			return 0;
		}

		return m_dwRef;
	}

	STDMETHODIMP CDetectionDlg::QueryInterface(REFIID iid, void FAR* FAR* ppvObj)
	{
		if (iid == IID_IUnknown || iid == IID_ISampleGrabberCB) {
			*ppvObj = this;
			AddRef();
			return NOERROR;
		}

		return E_NOINTERFACE;
	}

	STDMETHODIMP CDetectionDlg::SampleCB(double, IMediaSample*);
	STDMETHODIMP CDetectionDlg::BufferCB(double, BYTE*, long);

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	//int CountAllDevices();
	//void StartThisDevice(IMoniker *pmVideo);
	//BOOL StartPreview();
	//BOOL StopPreview();

protected:
	HICON m_hIcon;
	CComPtr<IGraphBuilder> m_gb;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CStatic _video;
};
