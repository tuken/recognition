#include "afxwin.h"
class CInfoDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CInfoDialog)

public:
	CInfoDialog(CWnd *pParent = NULL);
	virtual ~CInfoDialog();

	enum { IDD = IDD_INFO_DIALOG };

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLbnSelchangeLstCamera();

	void DetecttionCount(int count);
	//void SetCameraName(const wchar_t *name);
	void SetCameraList(CComPtr<IEnumMoniker>& mons);

	CStatic m_state;
	CStatic m_count;
	CStatic m_camera;
	CListBox m_listCam;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual void OnCancel();
	//virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

	CFont m_font1;
	CFont m_font2;
};
