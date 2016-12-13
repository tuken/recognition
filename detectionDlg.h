#pragma once
#include "afxwin.h"
#include "qedit.h"
#include <atlbase.h>
#include "infoDlg.h"

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

	afx_msg void OnClose();

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	HICON m_hIcon;
	CStatic m_video;
	CInfoDialog m_dlg;

	CComPtr<IGraphBuilder> m_gb;
	AM_MEDIA_TYPE m_mt;

	void *m_engine;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void OnIdle();

	bool m_once;
	ULONGLONG m_shot;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
