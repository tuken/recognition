#include "afxwin.h"
#include <string>

using namespace std;

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
	afx_msg void OnBnClickedBtnFolder();

	void DetectionCount(int count);
	void RecognitionInfo(const wstring& name, const float& score);
	//void SetCameraName(const wchar_t *name);
	void SetCameraList(CComPtr<IEnumMoniker>& mons);
	void SetImageList(CString& item);

	CStatic m_state;
	CStatic m_count;
	CStatic m_camera;
	CListBox m_listCam;
	CListBox m_listImg;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual void OnCancel();
	//virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()

	CFont m_font1;
	CFont m_font2;
	CString m_name;

	BOOL SelectFolder(HWND hWnd, LPCTSTR lpDefFolder, LPTSTR lpSelectPath, UINT iFlg, CString ttl);
};
