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

	void DetecttionCount(int count);
	void SetCameraName(const wchar_t *name);

	CStatic m_state;
	CStatic m_count;
	CStatic m_camera;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual void OnCancel();
	//virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

	CFont m_font1;
	CFont m_font2;
};
