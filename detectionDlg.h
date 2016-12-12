#pragma once
#include "afxwin.h"

class CDetectionDlg : public CDialogEx
{
public:
	CDetectionDlg(CWnd *pParent = NULL);

	enum { IDD = IDD_DETECTION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	//int CountAllDevices();
	//void StartThisDevice(IMoniker *pmVideo);
	//BOOL StartPreview();
	//BOOL StopPreview();

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic _video;
};
