#include "stdafx.h"
#include "Feedback.h"
#include "Luzj_ZTE.h"
#include "Luzj_ZTEDlg.h"
#include "SettingDlg.h"
#include "AutoUpdate.h"
CLuzj_ZTEDlg ZTEDlg;
Feedback::Feedback(CWnd* pParent /*=NULL*/)
	: CDialog(Feedback::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
void Feedback::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingDlg)
	DDX_Control(pDX, IDC_EXPLORER2, m_web);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(Feedback, CDialog)
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDOK, &CbaiduDlg::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL Feedback::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetIcon(ZTEDlg.m_hIcon, TRUE);			// Set big icon
	SetIcon(ZTEDlg.m_hIcon, FALSE);		// Set small icon
	
	char vaUrl[90] = _T("http://bbs.gxgk.cc/forum.php?mod=viewthread&tid=7203&extra=page%3D1");
	sprintf(vaUrl, "%s?User=%s", vaUrl, ZTEDlg.m_username);
	//m_web.Navigate2(vaUrl, &vt, &vt, &vt, &vt);
	m_web.Navigate2((COleVariant)vaUrl, NULL, NULL, NULL, NULL);
	return TRUE;
}