

/**************************************************************************************
			The Luzj's Zte Project
			//////////////////////
			Copyleft ? 2009 Luzj
		Author:Luzj		QQ:86829232
		http://blog.csdn.net/luzjqq
		Email: luzjcn@gmail.com
	///////////////////////////////////
关于Luzj's Zte认证端的声明：

1、本软件所有涉及的中兴认证的功能的实现均是通过黑盒分析得来，并未通过任何不正当方法获得。

2、本软件仅供研究学习之用，不得使用本软件损害中兴公司商业利益。

3、本软件不可用于任何商业和非法用途，否则责任自负。

4、本软件在发布前均通过一般性应用测试，但不保证任何情况下对机器无害，
由于未知的使用环境或不当的使用对计算机造成的损害，责任由使用者全部承担。

5.本软件版权没有，翻印不究，但请协助改进本作品。

6.本软件属开源软件，如果需要修改本软件源码以进行二次发布，也请继续公开源代码。

由于任何不遵守上叙条例引起的纠纷，均与本人无关，如不同意该声明请不要使用该软件，谢谢合作。
**************************************************************************************/
// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Luzj_ZTEDlg.h"
#include "SettingDlg.h"
#include "AutoUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg dialog


CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingDlg)
	DDX_Control(pDX, IDC_WEBURL, m_url);
	DDX_Control(pDX, IDC_AUTHOR_URL, m_author_url);
	DDX_Control(pDX, IDC_Modify_URL, m_modify_url);
	DDX_Control(pDX, IDC_ChangePassword, m_ChangePassword_url);
	DDX_Control(pDX, IDC_ChangeTYPassword, m_ChangeTYPassword_url);
	DDX_Control(pDX, IDC_Help, m_Help_url);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingDlg)
	ON_BN_CLICKED(IDC_CHK_WEB_AUTH, OnChkWebAuth)
	ON_BN_CLICKED(IDC_CHK_WEB_LOGOUT, OnChkWebLogout)
	ON_BN_CLICKED(IDC_CHK_ENABLE_WEBACCOUNT, OnChkEnableWebaccount)
	ON_BN_CLICKED(IDC_BTN_AUTO_UPDATE, OnBtnAutoUpdate)
	ON_BN_CLICKED(IDC_CHK_HTTP_HEART, OnChkHttpHeart)
	ON_BN_CLICKED(IDC_BTN_AUTO_WinPcapUPDATE, OnBtnAutoWinPcapUpdate)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHK_bTimingReauth, &CSettingDlg::OnBnClickedChkbtimingreauth)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingDlg message handlers

BOOL CSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CheckDlgButton(IDC_CHK_AUTOLOGON,Config.m_bAutologon?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTOWEBLOGON, Config.m_bAutoWeblogon ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTORUN,Config.m_bAutorun?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_BUBBLE,Config.m_bShowBubble?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WEB_AUTH,Config.m_bWebAuth?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_WEB_LOGOUT,Config.m_bWebLogout?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_ENABLE_WEBACCOUNT,Config.m_bEnableWebAccount?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTO_UPDATE,Config.m_bAutoUpdate?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_DEBUG,Config.m_bDebug?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_AUTO_FILTER,Config.m_bAutoFilter?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_DHCP,Config.m_bDHCP?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_HTTP_HEART,Config.m_bHttpHeart?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_Reauth, Config.m_Reauth ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_CHK_bTimingReauth, Config.m_bTimingReauth ? BST_CHECKED : BST_UNCHECKED);

	char szTemp[MAX_STRING];
	sprintf(szTemp,"%d",Config.m_iTimeout);
	GetDlgItem(IDC_TIMEOUT)->SetWindowText(szTemp);

	sprintf(szTemp,"%d",Config.m_iHeartInterval);
	GetDlgItem(IDC_HTTP_HEART_INTERVAL)->SetWindowText(szTemp);

	sprintf(szTemp, "%s", Config.m_csReauthTime);
	GetDlgItem(IDC_ReauthTime)->SetWindowText(szTemp);

	GetDlgItem(IDC_WEB_AUTH_URL)->SetWindowText(Config.m_csWebAuthUrl);
	GetDlgItem(IDC_WEB_LOGOUT_URL)->SetWindowText(Config.m_csWebLogoutUrl);
	GetDlgItem(IDC_WEB_USERNAME)->SetWindowText(Config.m_csWebUsername);
	GetDlgItem(IDC_WEB_PASSWORD)->SetWindowText(Config.m_csWebPassword);
	GetDlgItem(IDC_WEB_HEART_URL)->SetWindowText(Config.m_csHeartUrl);
	GetDlgItem(IDC_WEB_HEART_COOKIES)->SetWindowText(Config.m_csHeartCookies);
	
	m_url.SetURL(STR_WEB_URL); m_author_url.SetURL(STR_AUTHOR_URL); 
	m_modify_url.SetURL(STR_Modify_URL); 
	m_ChangePassword_url.SetURL(URL_ChangePassword); m_ChangeTYPassword_url.SetURL(URL_ChangeTYPassword); m_Help_url.SetURL(URL_Help);
	GetDlgItem(IDC_VERSION)->SetWindowText(STR_AppName"  "STR_Version);

	OnChkWebAuth();
	OnChkWebLogout();
	OnChkEnableWebaccount();
	OnChkHttpHeart();
	OnBnClickedChkbtimingreauth();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::OnOK()
{
	
#define BIND_BOOL_VAR_CONTROL(v, c) v=(IsDlgButtonChecked(c)==0)?false:true;
	BIND_BOOL_VAR_CONTROL(Config.m_bAutologon, IDC_CHK_AUTOLOGON);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoWeblogon, IDC_CHK_AUTOWEBLOGON);
	BIND_BOOL_VAR_CONTROL(Config.m_bWebAuth, IDC_CHK_WEB_AUTH);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutorun, IDC_CHK_AUTORUN);
	BIND_BOOL_VAR_CONTROL(Config.m_bShowBubble, IDC_CHK_BUBBLE);
	BIND_BOOL_VAR_CONTROL(Config.m_bWebLogout, IDC_CHK_WEB_LOGOUT);
	BIND_BOOL_VAR_CONTROL(Config.m_bEnableWebAccount, IDC_CHK_ENABLE_WEBACCOUNT);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoUpdate, IDC_CHK_AUTO_UPDATE);
	BIND_BOOL_VAR_CONTROL(Config.m_bDebug, IDC_CHK_DEBUG);
	BIND_BOOL_VAR_CONTROL(Config.m_bAutoFilter, IDC_CHK_AUTO_FILTER);
	BIND_BOOL_VAR_CONTROL(Config.m_bDHCP, IDC_CHK_DHCP);
	BIND_BOOL_VAR_CONTROL(Config.m_bHttpHeart, IDC_CHK_HTTP_HEART);
	BIND_BOOL_VAR_CONTROL(Config.m_Reauth,IDC_CHK_Reauth);
	BIND_BOOL_VAR_CONTROL(Config.m_bTimingReauth,IDC_CHK_bTimingReauth);

	char szTemp[MAX_STRING];
	GetDlgItem(IDC_TIMEOUT)->GetWindowText(szTemp,MAX_STRING);
	Config.m_iTimeout=atoi(szTemp);

	GetDlgItem(IDC_HTTP_HEART_INTERVAL)->GetWindowText(szTemp,MAX_STRING);
	Config.m_iHeartInterval = atoi(szTemp);

	GetDlgItem(IDC_ReauthTime)->GetWindowText(szTemp,MAX_STRING);
	Config.m_csReauthTime = szTemp;

#define BIND_STRING_VAR_CONTROL(v, c) {CString t; GetDlgItem(c)->GetWindowText(t); v = t;}

	BIND_STRING_VAR_CONTROL(Config.m_csWebAuthUrl, IDC_WEB_AUTH_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csWebLogoutUrl, IDC_WEB_LOGOUT_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csWebUsername, IDC_WEB_USERNAME);
	BIND_STRING_VAR_CONTROL(Config.m_csWebPassword, IDC_WEB_PASSWORD);
	BIND_STRING_VAR_CONTROL(Config.m_csHeartUrl, IDC_WEB_HEART_URL);
	BIND_STRING_VAR_CONTROL(Config.m_csHeartCookies, IDC_WEB_HEART_COOKIES);

	Config.SaveConfig();
	CDialog::OnOK();
}

void CSettingDlg::OnChkWebAuth() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_WEB_AUTH))
	{
		GetDlgItem(IDC_WEB_AUTH_URL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_AUTH_URL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkWebLogout() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_WEB_LOGOUT))
	{
		GetDlgItem(IDC_WEB_LOGOUT_URL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_LOGOUT_URL)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnChkEnableWebaccount() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_ENABLE_WEBACCOUNT))
	{
		GetDlgItem(IDC_WEB_USERNAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_WEB_PASSWORD)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_WEB_PASSWORD)->EnableWindow(FALSE);
	}
}

void CSettingDlg::OnBtnAutoUpdate() 
{
	// TODO: Add your control notification handler code here
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg *)parent;	
	int ret;
	if(Dlg != NULL) {
		ret = Dlg->CheckUpdate();
		if(ret == 2) {
			AfxMessageBox("这已经是最新版本的了！");
		}
		if (ret == -1) {
			AfxMessageBox("获取本地信息失败或远程服务器无法连接，检测更新失败，请手动更新！");
		}
	}
	
}

void CSettingDlg::OnBtnAutoWinPcapUpdate()
{
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg *)parent;
	int ret;
	if (Dlg != NULL) {
		ret = Dlg->CheckWinPcapUpdate();
		if (ret == 2) {
			AfxMessageBox("WinPcap已经是最新版本的了！");
		}
		if (ret == -1) {
			AfxMessageBox("远程服务器无法连接，请手动更新WinPcap！");
		}
	}
}

void CSettingDlg::OnChkHttpHeart() 
{
	// TODO: Add your control notification handler code here
	if(IsDlgButtonChecked(IDC_CHK_HTTP_HEART))
	{
		GetDlgItem(IDC_WEB_HEART_URL)->EnableWindow(TRUE);
		GetDlgItem(IDC_WEB_HEART_COOKIES)->EnableWindow(TRUE);
		GetDlgItem(IDC_HTTP_HEART_INTERVAL)->EnableWindow(TRUE);
	}else{
		GetDlgItem(IDC_WEB_HEART_URL)->EnableWindow(FALSE);
		GetDlgItem(IDC_WEB_HEART_COOKIES)->EnableWindow(FALSE);
		GetDlgItem(IDC_HTTP_HEART_INTERVAL)->EnableWindow(FALSE);
	}
}



void CSettingDlg::OnBnClickedChkbtimingreauth()
{
	if (IsDlgButtonChecked(IDC_CHK_bTimingReauth))
	{
		GetDlgItem(IDC_ReauthTime)->EnableWindow(TRUE);
	}
	else{
		GetDlgItem(IDC_ReauthTime)->EnableWindow(FALSE);
	}
}
