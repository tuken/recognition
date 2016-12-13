#include "stdafx.h"
#include "detection.h"
#include "infoDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CInfoDialog, CDialogEx)

CInfoDialog::CInfoDialog(CWnd* pParent /*=NULL*/)
: CDialogEx(CInfoDialog::IDD, pParent)
{
}

CInfoDialog::~CInfoDialog()
{
}

void CInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInfoDialog, CDialogEx)
END_MESSAGE_MAP()
