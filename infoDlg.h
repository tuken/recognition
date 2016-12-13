class CInfoDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CInfoDialog)

public:
	CInfoDialog(CWnd *pParent = NULL);
	virtual ~CInfoDialog();

	enum { IDD = IDD_INFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual void OnCancel();
	//virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
};
