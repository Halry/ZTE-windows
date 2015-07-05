
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



// Luzj_ZTEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Luzj_ZTEDlg.h"
#include "AutoUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLuzj_ZTEDlg dialog
CLuzj_ZTEDlg::CLuzj_ZTEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLuzj_ZTEDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLuzj_ZTEDlg)

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//认证状态为 未认证
	m_bAuth=FALSE;
}

void CLuzj_ZTEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLuzj_ZTEDlg)
	DDX_Control(pDX, IDC_USERNAME, _T(m_ccb_username));
	DDX_Control(pDX, IDC_NETCARD, _T(m_ccbNetCard));
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_Banner, Banner_Var);
}

BEGIN_MESSAGE_MAP(CLuzj_ZTEDlg, CDialog)
	//{{AFX_MSG_MAP(CLuzj_ZTEDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_LOGOFF, OnLogoff)	
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(MENU_SHOW, OnTrayShow)
	ON_MESSAGE(WM_USER_TRAY_NOTIFICATION,OnTrayNotification)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SETTING, OnSetting)
	ON_BN_CLICKED(IDC_LOGSHOW, OnLogshow)
	ON_CBN_SELCHANGE(IDC_USERNAME, OnSelchangeUsername)
	ON_BN_CLICKED(MENU_EXIT, OnExit)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, WebAuthButton)
	ON_BN_CLICKED(IDC_BUTTON2, WebAuthButton2)
	ON_STN_CLICKED(IDC_Banner, OnStnClickedBanner)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_Feedback, OnBnClickedFeedback)	
	ON_MESSAGE(WM_MSG_STATUS, OnStatus)
	ON_MESSAGE(WM_PING_END, OnPingEnd)
END_MESSAGE_MAP()


int CLuzj_ZTEDlg::CheckUpdate()
{
	int ret = AutoUpdate();
	if(ret == 0) {
		if(this->m_bAuth) {
			this->OnLogoff();
		}
		this->PostMessage(WM_QUIT);
	//	Sleep(1000);
		TerminateProcess(GetCurrentProcess(), 0);
	} else if(ret == 4) { return 2; }
	else if (ret == -1) { return -1; }
	return 1;
}
int CLuzj_ZTEDlg::CheckWinPcapUpdate()
{

	int ret = AutoUpdateWinPcap();
	if (ret == 0) {
		if (this->m_bAuth) {
			this->OnLogoff();
		}
		this->PostMessage(WM_QUIT);
		//	Sleep(1000);
		TerminateProcess(GetCurrentProcess(), 0);
	}
	else if (ret == 4) { return 2; }
	else if (ret == -1) { return -1; }
	return 1;
}
char *CLuzj_ZTEDlg::GetOSVersion()
{
	static char ver[MAX_STRING];
	OSVERSIONINFO os;
	strncpy(ver, "unknown", MAX_STRING);
	ZeroMemory(&os, sizeof(OSVERSIONINFO));
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&os)) {
		_snprintf(ver, MAX_STRING, "%d.%d.%d.%d.%s", 
			os.dwMajorVersion, os.dwMinorVersion, os.dwBuildNumber, 
			os.dwPlatformId, os.szCSDVersion);
	}
	return ver;
}

char *	CLuzj_ZTEDlg::GetGUID(const char *name)
{
	static char GUID[MAX_STRING];

	if(name == NULL) return NULL;
	const char *a = strstr(name, "{");
	if(a == NULL) return NULL;
	const char *b = strstr(a, "}");
	if(b == NULL) return NULL;

	memcpy(GUID, a, b - a + 1);
	GUID[b - a + 1] = '\0';
	return GUID;
}

char *	CLuzj_ZTEDlg::ToTCPName(const char *GUID)
{
	static char NPFGUID[MAX_STRING] = "\\Device\\Tcpip_";
	strncpy(NPFGUID + 14, GUID, MAX_STRING - 14); 
	
	return NPFGUID;
}

char *	CLuzj_ZTEDlg::ToNPFName(const char *GUID)
{
	static char TCPGUID[MAX_STRING] =  "\\DEVICE\\NPF_";
	strncpy(TCPGUID + 12, GUID, MAX_STRING - 12); 
	return TCPGUID;
}



BOOL CLuzj_ZTEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//读取配置到文件配置对象中去
	Config.LoadConfig();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SmallICON);
	SetIcon(m_hIcon, FALSE);		// Set small icon
	//Banner
	//Banner_Show();

	SetWindowText(STR_AppName);
	Config.m_iReauthlog = 1;
	status = INIT;
	m_bAuth = FALSE;
	m_DHCPThread = NULL;
	m_WebAuthThread = NULL;
	m_AuthThread = NULL;
	m_HttpHeartThread = NULL;
	m_NetStatus = NULL;
	editLog = (CEdit*)GetDlgItem(IDC_EDIT_LOG);

	//创建托盘图标
	m_tray.Create(this, WM_USER_TRAY_NOTIFICATION,STR_AppName, m_hIcon, 0);


	//修改列表控件
	GetWindowRect(&m_rc);

	m_rc.top=m_rc.bottom-5;    //设置状态栏的矩形区域
	m_StatusBar.Create(WS_CHILD |WS_VISIBLE|CBRS_BOTTOM,m_rc,this,20000);  
	int nParts[5]= {100,250,380,580,-1};      //分割尺寸
	m_StatusBar.SetParts(5, nParts);
	m_StatusBar.SetText(_T(""), 0, 0);
	m_StatusBar.SetText(_T(""), 1, 0);
	m_StatusBar.SetText(_T(""), 2, 0);
	m_StatusBar.SetText(_T(""), 3, 0);
	m_StatusBar.SetText(_T(""), 4, 0);
	//控制状态栏的显示
	SetTimer(1,1000,NULL);
	//开始的时候先将日志框隐藏
	OnLogshow();

	Log(I_MSG, "软件版本:%s", STR_Version);
	Log(I_MSG, "Winpcap版本:%s", pcap_lib_version());
	Log(I_MSG, "操作系统版本：%s", GetOSVersion());

	//////////////////////////////////////////////////////////////////////////	
	int i=0,k=0;
	CString str;
	
	//if(Config.m_bAutoUpdate) CheckUpdate();

	CheckDlgButton(IDC_REMEMBER,Config.m_bRememberPWD?BST_CHECKED:BST_UNCHECKED);
	//////////////////////////////////////////////////////////////////////////
	//加载账号信息

	CString user,pass;	
	POSITION p = Config.m_UserInfo.GetStartPosition();	
	while(p != NULL) {
		Config.m_UserInfo.GetNextAssoc(p, user, pass);		
		m_ccb_username.AddString(user);		
	}
	k = m_ccb_username.FindStringExact(-1, Config.m_csLastUser);
	if(k < 0) k = 0;
	if(!Config.m_UserInfo.IsEmpty()) {
		m_ccb_username.SetCurSel(k);
		m_ccb_username.GetWindowText(user);
		GetDlgItem(IDC_PWD)->SetWindowText(Config.m_UserInfo[user]);		
	}
	/*
	FILE *fp;
	if ((fp = fopen("zte.conf", "r")) == NULL)//判断保存的文件是否存在，r表示只读
	{
		Config.SaveConfig();
	}
	*/
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//加载网卡信息	
	char errorBuffer[ PCAP_ERRBUF_SIZE ];		//错误信息缓冲区
	pcap_if_t		* allAdapters;				//适配器列表
	if(pcap_findalldevs(&allAdapters, errorBuffer) == -1 || allAdapters == NULL)
	{
		int result = MessageBox(_T("读取网卡信息失败，可能WinPcap没有安装!是否安装WinPcap？"), _T("错误"), MB_ICONQUESTION | MB_YESNO);
		pcap_freealldevs(allAdapters);
		switch (result)/*注意！使用Unicode应用TEXT包围字串*/
		{
		case IDYES:
		{	
			if ((_access("WinPcap.exe", 0)) != -1)
			{
				::ShellExecute(this->GetSafeHwnd(), _T("open"), _T(".\\WinPcap.exe"), NULL, NULL, SW_SHOWNORMAL);
				CDialog::OnCancel();break;
			}
			else
				AfxMessageBox(_T("找不到WinPcap安装包，请手动安装！"));
				CDialog::OnCancel(); break;
		}
		case IDNO:
			CDialog::OnCancel(); break;
		}
	}
	k = 0; m_csAdapters.RemoveAll();
	pcap_if_t* adapter;//临时存放适配器
	char *szGuid = NULL;	
	Log(I_INFO, "LastNetcard:%s", Config.m_csNetCard);
    for(k = 0, adapter = allAdapters; adapter != NULL; adapter = adapter->next) {
		if(adapter->flags & PCAP_IF_LOOPBACK) continue;	
		szGuid = GetGUID(adapter->name);
		if ((Config.m_bAutoFilter == 1) && (TestAdapter(szGuid) != 0) || (Config.m_bAutoFilter == 1 && strcmp(adapter->description, "Microsoft") == 0))
			continue;
		Log(I_INFO, "load netcard:(%d)%s(%s)", k, szGuid, adapter->description);
		m_csAdapters.Add(szGuid); m_ccbNetCard.AddString(adapter->description);
		k++;
    }
	pcap_freealldevs(allAdapters);
	if (strlen(Config.m_csNetCard) == NULL)
	{
		Config.m_csNetCard = m_csAdapters[0];	//无网卡时自动选择第一张网卡
	}
	for(k = m_ccbNetCard.GetCount() - 1; k >= 0; k--) 
	{
		if (stricmp(m_csAdapters[k], Config.m_csNetCard) == 0)
		{
			i = 1;
			break;
		}
	}
	if (i == 0)
	{
		k = 0;
	}
	if (m_ccbNetCard.GetCount() == 0)
	{
		this->Log(I_ERR, "网卡选项为空，请检查是否安装网卡驱动和是否禁用网卡，并重新启动程序！！");
	}
	if (k >= 0 && m_ccbNetCard.GetCount() > k) {
		m_ccbNetCard.SetCurSel(k);
		Log(I_INFO, "select netcard:(%d)%s", k, m_csAdapters[k]);
	}

	this->Log(I_MSG, "加载网卡完成");

	//////////////////////////////////////////////////////////////////////////

	//使得开始按钮有效，而断开按钮无效
	UpdateStatus(FALSE);
	//自动登陆选项
	if (k >= 0 && Config.m_bAutologon == TRUE && Config.m_bAutoWeblogon == FALSE)
	{ 
		OnStart(); 
	}
	if (k >= 0 && Config.m_bAutologon == TRUE && Config.m_bAutoWeblogon == TRUE)
	{
		WebAuthButton();
	}
	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLuzj_ZTEDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_MINIMIZE || (nID & 0xFFF0) == SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
		SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
		SetBubble("提示", STR_AppName" 正在后台运行着……", 1);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

char *CLuzj_ZTEDlg::GetAdapterInfo(const char *name)
{
	static char info[MAX_STRING];
	char *adaptername = ToTCPName(name);
	char temp[MAX_STRING];
	
	if(adaptername == NULL) return NULL;

	_snprintf(temp, MAX_STRING, "adapter name:%s\r\n", adaptername); strncpy(info, temp, MAX_STRING);
	
	PIP_ADAPTER_INFO AdapterInfo = NULL;

	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	DWORD dwStatus;
	AdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
	if(AdapterInfo == NULL) return "GetAdapterInfo:malloc1 failed";
	dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);	
	if(dwStatus == ERROR_BUFFER_OVERFLOW) {
		free(AdapterInfo); 
		AdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
		if(AdapterInfo == NULL) return "GetAdapterInfo:malloc2 failed";
		dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);			
	}
	if(dwStatus != NO_ERROR) {
		if(AdapterInfo != NULL) free(AdapterInfo);
		return "GetAdapterInfo:GetAdaptersInfo failed";
	}	
	
	if(name != NULL) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;		
		while(pAdapterInfo) {
			if (stricmp(name,pAdapterInfo->AdapterName) == 0) {
				_snprintf(temp, MAX_STRING, "Descript:%s\r\n", pAdapterInfo->Description); 
				strncpy(info, temp, MAX_STRING);

				_snprintf(temp, MAX_STRING, "\tDHCP Enabled:%s", pAdapterInfo->DhcpEnabled ? "YES" : "NO" ); 
				strcat(info, temp); strcat(info, "\r\n");				

				_snprintf(temp, MAX_STRING, "\tIP:%s", pAdapterInfo->IpAddressList.IpAddress.String); 
				strcat(info, temp); strcat(info, "\r\n");

				_snprintf(temp, MAX_STRING, "\tMASK:%s", pAdapterInfo->IpAddressList.IpMask.String); 
				strcat(info, temp); strcat(info, "\r\n");

				_snprintf(temp, MAX_STRING, "\tGateWay:%s", pAdapterInfo->GatewayList.IpAddress.String); 
				strcat(info, temp); strcat(info, "\r\n");

				if(MIB_IF_TYPE_ETHERNET != pAdapterInfo->Type) {
					_snprintf(temp, MAX_STRING, "\tType:%d [warning]", pAdapterInfo->Type); 
					strcat(info, temp); strcat(info, "\r\n");
				}

				_snprintf(temp, MAX_STRING, "\tDHCP Server:%s", pAdapterInfo->DhcpServer.IpAddress.String); 
				strcat(info, temp);

				//开始ping
				m_pingThread.Pingstop = 0;
				m_pingThread.StartPing(0, "172.16.254.167", m_hWnd);
				break;
			}
			pAdapterInfo = pAdapterInfo->Next;
		};
		if(pAdapterInfo && Config.m_bDHCP && !pAdapterInfo->DhcpEnabled) {
			Log(I_WARN, "DHCP Config not match ! ");
			Log(I_MSG, "DHCP is set to fetch IP. ");
		}
	} else {
		return "GetAdapterInfo:DescriptionToName failed";
	}
	free(AdapterInfo);
	return info;
}

void CLuzj_ZTEDlg::SetBubble(char * title,char * content,int timeout)
{
	if (Config.m_bShowBubble==TRUE)
	{
		m_tray.SetBubble(title,content,timeout);
	}
}

void CLuzj_ZTEDlg::OnTrayShow()
{   
	if (IsWindowVisible())
	{
		ShowWindow(SW_HIDE);
		SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
	} 
	else
	{
		ShowWindow(SW_SHOWNA);
		BringWindowToTop();
	}
}

LONG CLuzj_ZTEDlg::OnTrayNotification(WPARAM wparam, LPARAM lparam)
{   
	CMenu	m_menu;
	CPoint	m_point;
	switch (lparam)
	{
	case WM_RBUTTONDOWN:
		m_menu.CreatePopupMenu();
		if (IsWindowVisible())
		{
			m_menu.AppendMenu(MF_ENABLED, MENU_SHOW,_T("隐藏窗口"));
		} 
		else
		{
			m_menu.AppendMenu(MF_ENABLED, MENU_SHOW,_T("显示窗口"));
		}
		m_menu.AppendMenu(MF_ENABLED, MF_SEPARATOR);
		m_menu.AppendMenu(MF_ENABLED, MENU_EXIT,_T("退 出"));
		GetCursorPos(&m_point);
		m_menu.TrackPopupMenu(TPM_LEFTALIGN,m_point.x,m_point.y,this);
		break;
	case WM_LBUTTONDOWN:
		if (IsWindowVisible())
		{
			ShowWindow(SW_HIDE);
			SetProcessWorkingSetSize(GetCurrentProcess(),-1,-1);
		}
		else
		{
			ShowWindow(SW_SHOWNA);
			BringWindowToTop();
		}	
		break;
	}
	return 0;
}

void CLuzj_ZTEDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLuzj_ZTEDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLuzj_ZTEDlg::OnStart() 
{
	//////////////////////////////////////////////////////////////////////////
	//自动更改为自动获取ip
	if (DHCP_Auto()==0)return;

	UpdateStatus(TRUE);

	//////////////////////////////////////////////////////////////////////////
	//		先存放设置参数
	CString strTemp;

	//取得用户名密码
	GetDlgItem(IDC_USERNAME)->GetWindowText((char*)m_username, sizeof(m_username));
	m_usernameLen=strlen((char*)m_username);
	GetDlgItem(IDC_PWD)->GetWindowText((char*)m_password, sizeof(m_password));
	m_passwordLen=strlen((char*)m_password);

	if (m_usernameLen<1 || m_passwordLen<1)	{
		this->Log(I_ERR, "用户名或者密码太短!"); UpdateStatus(FALSE); return;	
	}
	if (((CComboBox*)GetDlgItem(IDC_NETCARD))->GetCurSel()== -1)
	{
		this->Log(I_ERR, "选择网卡错误!"); UpdateStatus(FALSE); return;
	}//避免网卡没有选择而导致遇到未知参数而崩溃
	Config.m_csNetCard=m_csAdapters[((CComboBox*)GetDlgItem(IDC_NETCARD))->GetCurSel()];

	if (IsDlgButtonChecked(IDC_REMEMBER)) {		
		Config.m_bRememberPWD=TRUE;
	} else {		
		Config.m_bRememberPWD=FALSE;
	}

	Config.m_csLastUser=(char*)m_username;
	if(Config.m_bRememberPWD)
		Config.m_UserInfo[(char*)m_username]=(char*)m_password;

	Config.SaveConfig();
	//////////////////////////////////////////////////////////////////////////
	SetBubble("提示",STR_AppName" 开始联网认证……");
		
	char m_errorBuffer[PCAP_ERRBUF_SIZE];
	int retcode = 0;
		
	/////////////////////////////////////////////////////////////////////////
	//寻找所选的网卡的MAC	
	if (GetMacIP(Config.m_csNetCard, NULL, m_MacAdd) != 0)	
	{
		if(m_MacAdd[0] == 0 && m_MacAdd[1] == 0 && m_MacAdd[2] == 0 && 
			m_MacAdd[3] == 0 && m_MacAdd[4] == 0 && m_MacAdd[5] == 0) 
		{
			Log(I_ERR, "获取不到所选网卡的MAC地址"); UpdateStatus(FALSE);
			return;
		}
	} 
	else
	{
		Log(I_INFO, "MAC:%02X-%02X-%02X-%02X-%02X-%02X", m_MacAdd[0], m_MacAdd[1], m_MacAdd[2],
			m_MacAdd[3], m_MacAdd[4], m_MacAdd[5]);
	}
	//////////////////////////////////////////////////////////////////////////
	// 打开指定适配器
	m_adapterHandle=pcap_open_live(ToNPFName(Config.m_csNetCard),65536,1,Config.m_iTimeout,m_errorBuffer);
    if(m_adapterHandle == NULL) {	
		Log(I_INFO, "pcap_open_live:%s", m_errorBuffer); UpdateStatus(FALSE);
		return;
    }

	char	FilterStr[100];		//包过滤字符串
	struct bpf_program	mfcode;	

	sprintf(FilterStr, "(ether proto 0x888e) and (ether dst host %02x:%02x:%02x:%02x:%02x:%02x)",
			m_MacAdd[0],m_MacAdd[1],m_MacAdd[2],m_MacAdd[3],m_MacAdd[4],m_MacAdd[5]);

    if((retcode=pcap_compile(m_adapterHandle, &mfcode, FilterStr, 1, 0xff))==-1
		||(retcode=pcap_setfilter(m_adapterHandle, &mfcode))==-1)
    {
		Log(I_INFO, "pcap_compile & pcap_setfilter:%s", pcap_strerror(retcode));
		pcap_close(m_adapterHandle); UpdateStatus(FALSE);
		return;
    }

	status = AUTHING;

	//////////////////////////////////////////////////////////////////////////
	//服务类控制
	int msg;
	if (Config.m_bDHCP)
	{
		if (msg = StartService(_T("Dhcp")) == SERVICE_RUNNING)
			Log(I_MSG, "检测到DHCP服务停止，DHCP服务启用成功");//启用系统DHCP服务
		else if (msg != 0)
			Log(I_MSG, "DHCP服务启动失败，可能获取不到IP,错误代码:%d", msg);
	}
	if (msg = StopService(_T("dot3svc")) == SERVICE_ACCEPT_STOP)
		Log(I_MSG, "停止系统自带802.1x服务成功,避免十几分钟断线重连");//停用系统自带802.1x服务
	else if (msg != 0)
		Log(I_MSG, "停止系统自带802.1x服务失败,可能出现十几分钟断线重连,错误代码:%d",msg);
	if (msg = StopService(_T("SharedAccess")) == SERVICE_ACCEPT_STOP)
	{
		ShareServiceStatus = 1;
		Log(I_MSG, "停止系统ICS服务成功,避免获取IP失败，影响Wifi共享功能");//停用系统ICS服务
	}
	else if (msg != 0)
		Log(I_MSG, "停止系统ICS服务失败,获取IP失败可能失败,错误代码:%d", msg);
	//////////////////////////////////////////////////////////////////////////
	CPacket packet;
	//////////////////////////////////////////////////////////////////////////
	///开始认证包
    if(!(retcode=packet.send_packet_start(m_adapterHandle,m_MacAdd))) {   
		Log(I_INFO, "send_packet_start:(%d)%s", retcode, pcap_geterr(m_adapterHandle));	 UpdateStatus(FALSE);	
		return;
    }
	Log(I_MSG, "中兴认证开始...");
	Log(I_INFO, "Client:EAPOL_START...");

	m_AuthThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)eap_thread, this, 0, 0);
	if(m_AuthThread == NULL) Log(I_INFO, "CreateThread:(%d)", GetLastError());
}

DWORD WINAPI CLuzj_ZTEDlg::eap_thread(void *para)
{
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg*)para;	

	pcap_loop(Dlg->m_adapterHandle,-1,CLuzj_ZTEDlg::get_packet,(u_char*)Dlg);

	pcap_close(Dlg->m_adapterHandle);
	return 0;
}

DWORD WINAPI CLuzj_ZTEDlg::dhcp_thread(void *para)
{
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg*)para;
	
	Dlg->status = DHCPING;

	int retcode = 0;

	Dlg->Log(I_INFO, "获取IP线程启动");

	if(Config.m_bDHCP) {
		retcode = Dlg->IpconfigRenew();	
		if(retcode != 0) Dlg->Log(I_INFO, "Ipconfig/Renew return %d", retcode);
		else Dlg->status = DHCPED;	
	}
	
	if (retcode == 0 && (Config.m_bWebAuth && Config.m_csWebAuthUrl.GetLength() > 0) && Dlg->m_bAuth){
		Dlg->status = HTTPING;
		Dlg->StartWebAuth();
		//if(Config.m_bHttpHeart)	Dlg->OnHttpHeart();	
	}

	Dlg->Log(I_INFO, "停止尝试获取IP.");

	Dlg->m_DHCPThread = NULL;
		
	return 0;
}


DWORD WINAPI CLuzj_ZTEDlg::GetMacIP(const char *adaptername, char *ip, unsigned char *mac)
{
	PIP_ADAPTER_INFO AdapterInfo = NULL;

	DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
	DWORD dwStatus;
	AdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
	if(AdapterInfo == NULL) return -1;
	dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);	
	if(dwStatus == ERROR_BUFFER_OVERFLOW) {
		free(AdapterInfo); 
		AdapterInfo = (PIP_ADAPTER_INFO)malloc(dwBufLen);
		dwStatus = GetAdaptersInfo(AdapterInfo,&dwBufLen);	
		if(AdapterInfo == NULL) return -1;
	}
	if(dwStatus != NO_ERROR) {
		if(AdapterInfo != NULL) free(AdapterInfo);
		return -2;
	}

	if(ip) memset(ip, 0, 16);	
	if(mac) memset(mac, 0, 6);

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;	
	if(adaptername != NULL) {			
		while(pAdapterInfo) {
			if (stricmp(adaptername,pAdapterInfo->AdapterName) == 0) {
				if(mac) memcpy(mac, pAdapterInfo->Address, 6);
				if(ip) strncpy(ip, pAdapterInfo->IpAddressList.IpAddress.String, 16);
				break;
			}
			pAdapterInfo = pAdapterInfo->Next;
		};
	}
	
	free(AdapterInfo);
	if(pAdapterInfo == NULL) return -3;
	
	return 0;
}

#define MAX_DHCP_TIMES	10

DWORD WINAPI CLuzj_ZTEDlg::IpconfigRenew()
{
	int i;
	DWORD ret;
	char  adaptername[MAX_ADAPTER_NAME];
	int count = 0;

	DWORD dwOutBufLen = sizeof(IP_INTERFACE_INFO);
	PIP_INTERFACE_INFO pIfTable = (PIP_INTERFACE_INFO)malloc(dwOutBufLen);
	if(pIfTable == NULL) return -1;
	
	ret = GetInterfaceInfo(pIfTable, &dwOutBufLen);
	if(ret == ERROR_INSUFFICIENT_BUFFER) {
		free(pIfTable); pIfTable = (PIP_INTERFACE_INFO)malloc(dwOutBufLen);
		if(pIfTable == NULL) return -2;
		ret = GetInterfaceInfo(pIfTable, &dwOutBufLen);
	}

	if(ret != NO_ERROR) return -3;

	for(i = 0; i < pIfTable->NumAdapters; i++) {
		wcstombs(adaptername, pIfTable->Adapter[i].Name, MAX_ADAPTER_NAME);

		if(stricmp(adaptername, ToTCPName(Config.m_csNetCard)) == 0) {

			EnableDHCP(GetGUID(adaptername), true);

			while (status != OFFLINE) {
				if (count <= MAX_DHCP_TIMES) Log(I_MSG, "正在获取IP地址...");
				IpReleaseAddress(&pIfTable->Adapter[i]);
				ret = IpRenewAddress(&pIfTable->Adapter[i]);
				if(ret == NO_ERROR) break;	
				if(count <= MAX_DHCP_TIMES) {
					if(count == MAX_DHCP_TIMES) {
						count++;
						Log(I_MSG, "达到获取IP次数尝试上限，测试是否联网");
						break;
					} else {
						Log(I_WARN, "获取IP地址失败,错误代码：%d，请检查是否手动设置IP，并重新插入网线或者重启网卡", ret);
						count++;
					}
				} 
				Sleep(10000);
			}
			char *info = GetAdapterInfo((LPCSTR)Config.m_csNetCard);
			if(info == NULL) Log(I_INFO, "无适配器信息，请确认您选择了网卡！");
			else Log(I_INFO, info);		

			break;
		}	
	}
	free(pIfTable);
	return 0;
}

typedef BOOL (WINAPI *DHCPNOTIFYPROC)(
    LPWSTR lpwszServerName, // 本地机器为NULL
    LPWSTR lpwszAdapterName, // 适配器名称
    BOOL bNewIpAddress, // TRUE表示更改IP
    DWORD dwIpIndex, // 指明第几个IP地址，如果只有该接口只有一个IP地址则为0
    DWORD dwIpAddress, // IP地址
    DWORD dwSubNetMask, // 子网掩码
    int nDhcpAction ); // 对DHCP的操作 0:不修改, 1:启用 DHCP，2:禁用 DHCP


bool CLuzj_ZTEDlg::EnableDHCP(const char* szAdapterName, const bool enable)
{
	bool			bResult = false;
	HINSTANCE		hDhcpDll;
	DHCPNOTIFYPROC	pDhcpNotifyProc;
	WCHAR wcAdapterName[256];

	//mbstowcs(wcAdapterName, szAdapterName, strlen(szAdapterName));
	MultiByteToWideChar(CP_ACP, 0, szAdapterName, -1, wcAdapterName,256);
			
	if ((hDhcpDll = LoadLibrary(_T("dhcpcsvc.dll"))) == NULL)
		return false;

	if((pDhcpNotifyProc = (DHCPNOTIFYPROC)GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange")) != NULL) {
		if(pDhcpNotifyProc(NULL, wcAdapterName, FALSE, 0, 0, 0, enable?1:2) == ERROR_SUCCESS) 
		{
			bResult = true;
		}
	}
	FreeLibrary(hDhcpDll);
	return bResult;
}

void CLuzj_ZTEDlg::Log (int level, const char *fmt, ...)
{

	if(level == I_INFO && !Config.m_bDebug) return;

    va_list args;
    char msg[MAX_STRING];
    va_start (args, fmt);
    vsnprintf (msg, MAX_STRING, fmt, args);
    va_end (args);

	time_t t=time(NULL);
	char szTime[MAX_STRING];
	strftime(szTime,MAX_STRING,"%H:%M:%S",localtime(&t));
	strcat(szTime, " "); strcat(szTime, msg); strcat(szTime, "\r\n");

	int nLength = editLog->SendMessage(WM_GETTEXTLENGTH);
    editLog->SetSel(nLength, nLength);
    editLog->ReplaceSel(szTime);
	editLog->SendMessage(WM_VSCROLL,SB_BOTTOM,0);
	
	if(level == I_WARN) SetBubble("Warnning", szTime);
	if (level == I_ERR) AfxMessageBox(msg, MB_ICONSTOP, 0);
}

char * CLuzj_ZTEDlg::HttpAuth(BOOL bForce = FALSE)
{
	char *msg = NULL;
	if((Config.m_bWebAuth && Config.m_csWebAuthUrl.GetLength() > 0) || bForce) {
		if(Config.m_bEnableWebAccount) {
			msg = WebAuth(Config.m_csWebUsername, Config.m_csWebPassword, m_ip, Config.m_csWebAuthUrl, Config.m_iTimeout);
		} else {
			msg = WebAuth((const char *)m_username, (const char *)m_password, m_ip, Config.m_csWebAuthUrl, Config.m_iTimeout);
		}
		//if(strcmp("请求认证超时", msg) == 0) Config.m_iTimeout++;
	}
	return msg;
}

void CLuzj_ZTEDlg::UpdateStatus(bool bOnline)
{
	
	const char *m2[] = {"Offline", "Online"};
	m_bAuth = bOnline;
	int i = (bOnline ? 1 : 0);
		
	this->Log(I_INFO, "status:%s", m2[i]);

	if(m_bAuth) {
		m_startTime = time(NULL);
		//ShowWindow(SW_HIDE);
	}

	GetDlgItem(IDC_USERNAME)->EnableWindow(!bOnline);
	GetDlgItem(IDC_PWD)->EnableWindow(!bOnline);
	GetDlgItem(IDC_NETCARD)->EnableWindow(!bOnline);
	GetDlgItem(IDC_START)->EnableWindow(!bOnline);
	GetDlgItem(IDC_Feedback)->EnableWindow(bOnline);
	GetDlgItem(IDC_LOGOFF)->EnableWindow(bOnline);

}

void CLuzj_ZTEDlg::get_packet(u_char *args, const struct pcap_pkthdr *pcaket_header, const u_char *packet)
{
	int retcode;
	static int good = 0;
	char msg[300];
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg*)args;
	/* declare pointers to packet headers */
	CPacket P;
	if(packet[15] == 0x00) 
	{ //eap_packet
		if (packet[18] == 0x01)
		{//request
			if (packet[22] == 0x01)
			{ //identity
				Dlg->Log(I_INFO, "Server:Request Identity...");
				if (!(retcode = P.send_packet_response_Identity(Dlg->m_adapterHandle, packet, Dlg->m_MacAdd, Dlg->m_username, Dlg->m_usernameLen)))
				{
					Dlg->Log(I_INFO, "send_packet_response_Identity:(%d)%s", retcode, pcap_geterr(Dlg->m_adapterHandle));
					Dlg->UpdateStatus(FALSE); return;
				}
				Dlg->Log(I_INFO, "Client:Response Identity:%s", Dlg->m_username);
			}
			else
				if (packet[22] == 0x04) {//md5 challenge
					Dlg->Log(I_INFO, "Server:Request MD5 Chanllenge...");
					if (!(retcode = P.send_packet_response_MD5(Dlg->m_adapterHandle, packet, Dlg->m_MacAdd, Dlg->m_username, Dlg->m_usernameLen, Dlg->m_password, Dlg->m_passwordLen)))
					{
						Dlg->Log(I_INFO, "send_packet_response_MD5:(%d)%s", retcode, pcap_geterr(Dlg->m_adapterHandle));
						Dlg->UpdateStatus(FALSE); return;
					}
					Dlg->Log(I_INFO, "Client:Response MD5 Chanllenge");
				}
				else
					if (packet[22] == 0x02)
					{//notification
						strcpy(msg, (char*)packet + 23);
						if (strstr(msg, "http")){
							char *p;
							p=strstr(msg,"\004");
							strcpy(p, "\0");
							Dlg->Log(I_MSG, "服务器信息:%s", msg + 2);
						}
						else
							Dlg->Log(I_MSG, "服务器信息:%s", msg);
					}
					else
					{//unknown
						Dlg->Log(I_INFO, "Unknown EAPOL Type:0x%02X", packet[22]);
					}
		}
		else 
			if (packet[18] == 0x03)
			{//successful
				Dlg->status = AUTHED;
				SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
				Dlg->UpdateStatus(TRUE);
				Dlg->Log(I_MSG, "中兴认证成功.");
	
				if (Dlg->m_DHCPThread == NULL)
					Dlg->m_DHCPThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)dhcp_thread, Dlg, 0, 0);
				Dlg->ShowWindow(SW_HIDE);
			}
			else 
				if (packet[18] == 0x04)
				{//fail
					int ErrorKind;
					Dlg->UpdateStatus(FALSE);
					if (strlen(msg) != 0 && (strstr(msg, "http") == 0) && (strstr(msg, "错误码") != 0))
					{
						Dlg->Log(I_ERR, "服务器认证失败,%s", msg);
						ErrorKind = 0;
					}
					else
					{
						Dlg->Log(I_ERR, "服务器认证失败,请查看日志，并尝试重新连接！", msg);
						ErrorKind = 1;
					}
					Dlg->FaultMsg(msg);
					Dlg->status = OFFLINE;
					memset(msg, 0, sizeof(msg));
					strcpy(msg, (char*)packet + 24);
					strcpy(&msg[9], "\0");
					if (strlen(msg)!=NULL){ 
						if (strstr(msg, "port") != 0){
							Dlg->Log(I_INFO, msg + 2);
						}
					}
					Dlg->m_pingThread.Pingstop = 1;
					Dlg->m_StatusBar.SetText("", 4, 0);
					/*
					if (Config.m_Reauth&&ErrorKind)
					{
						if (Config.m_iReauthlog = 1){
							Config.m_iReauthlog = 0;
							Dlg->SendMessage(WM_COMMAND, MAKEWPARAM(IDC_START, 0));
						}
					}*/
					ExitThread(0);
					return;
				}
			else 
			{//unknown
				Dlg->Log(I_INFO, "Unknown EAP Type:0x%02X", packet[18]);
			}
	} else if(packet[15] == 0x03) {//key		
		if(!(retcode=P.send_packet_key2(Dlg->m_adapterHandle,packet,Dlg->m_MacAdd))) {
			Dlg->Log(I_INFO, "send_packet_key2:(%d)%s", retcode, pcap_geterr(Dlg->m_adapterHandle));
			Dlg->UpdateStatus(FALSE);  return;
		}	
		if(good <= 3) Dlg->Log(I_INFO, good++ == 3 ? "正常在线." : "send_packet_key2...");	
		if(good >= 3) Dlg->status = ONLINE;

		//Dlg->HttpAuth();
	} else {//unknown
		Dlg->Log(I_INFO, "Unknown Type:0x%02X", packet[15]);
	}
    return;
}

//========================OnLogoff=====================================
void CLuzj_ZTEDlg::OnLogoff()
{
	if ((status == ONLINE || status == HTTPED) && Config.m_bWebLogout && strlen(Config.m_csWebLogoutUrl) > 0) {
		char *msg = WebLogout(this->m_ip, Config.m_csWebLogoutUrl, Config.m_iTimeout);
		if (strcmp(msg, "logoutsuccess") == 0)
		{
			this->Log(I_MSG, "网页认证已下线!");
		}
		else if (strcmp(msg, "logoutfailed") == 0)
		{
			this->Log(I_MSG, "网页认证下线失败");
		}
		else
		{
			this->Log(I_MSG, "网页认证下线异常，错误信息:%s", msg);
		}
	}
	CPacket packet;
	packet.send_packet_logoff(m_adapterHandle, m_MacAdd);
	pcap_breakloop(m_adapterHandle);
	//pcap_close(m_adapterHandle);

	//等待，如果500ms后没有自动退出，则强制结束
	if (::WaitForSingleObject(this->m_HttpHeartThread, 500) == WAIT_TIMEOUT)
	{
		::TerminateThread(this->m_HttpHeartThread, 0); this->m_HttpHeartThread = NULL;
	}

	if (::WaitForSingleObject(this->m_AuthThread, 500) == WAIT_TIMEOUT)
	{
		::TerminateThread(this->m_AuthThread, 0); this->m_AuthThread = NULL;
	}


	if (::WaitForSingleObject(this->m_DHCPThread, 500) == WAIT_TIMEOUT)
	{
		::TerminateThread(this->m_DHCPThread, 0); this->m_DHCPThread = NULL;
	}
	if (::WaitForSingleObject(this->m_NetStatus, 500) == WAIT_TIMEOUT)
	{
		::TerminateThread(this->m_NetStatus, 0); this->m_NetStatus = NULL;
		m_StatusBar.SetText(_T(""), 3, 0);
	}	
	m_pingThread.Pingstop = 1;
	this->UpdateStatus(FALSE);

	Log(I_MSG, "中兴认证注销.");
	this->status = OFFLINE;
}
//========================OnLogoff======================================


//////////////////////////////////////////////////////////////////////////
//响应退出按钮，如果已经认证，就先发送退出认证包
void CLuzj_ZTEDlg::OnExit() 
{
	ShowWindow(SW_HIDE);
	if (m_bAuth) {		
		this->OnLogoff();

	}
	
	CDialog::OnOK();
}
//////////////////////////////////////////////////////////////////////////


void CLuzj_ZTEDlg::OnTimer(UINT nIDEvent)
{
	static char *csStatus[] = { "初始化完毕", "中兴认证认证中", "中兴认证成功", "正在获取IP地址", "成功获取IP地址",
		"网页认证中", "网页认证成功", "在线", "离线" };

	static STATUS lastStatus = INIT;

	time_t t = time(NULL);
	char szTime[MAX_STRING];
	strftime(szTime, MAX_STRING, "%H:%M:%S", localtime(&t));
	m_StatusBar.SetText(szTime, 0, 0);

	if (m_bAuth == 0 && Config.m_bTimingReauth && stricmp(szTime, Config.m_csReauthTime) == 0) {
		//OnLogoff();
		SendMessage(WM_COMMAND, MAKEWPARAM(IDC_START, 0));
	}

	if (m_bAuth) {
		t -= m_startTime;
		strftime(szTime, MAX_STRING, "在线时间%H:%M:%S", gmtime(&t));
		m_StatusBar.SetText(szTime, 1, 0);
	}
	else {
		struct tm *t;
		time_t tt;
		time(&tt);
		t = localtime(&tt);
		sprintf(szTime, "%4d年%02d月%02d日", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
		m_StatusBar.SetText(szTime, 1, 0);
	}

	m_StatusBar.SetText(csStatus[status], 2, 0);

	if (lastStatus != status) {
		lastStatus = status;
		SetBubble("状态", csStatus[status]);
	}

	CDialog::OnTimer(nIDEvent);
}

void CLuzj_ZTEDlg::OnLogshow()
{
	static int bShow = TRUE;
	RECT rc1={0},rc2={0};
	if (bShow)
	{
		GetDlgItem(IDC_SPLIT)->GetWindowRect(&rc1);
		GetWindowRect(&m_rc);
		rc2=m_rc;
		rc2.right=rc1.left-2;
		MoveWindow(&rc2,TRUE);
		bShow=!bShow;
		GetDlgItem(IDC_LOGSHOW)->SetWindowText(_T("日志>>"));
	}
	else
	{
		GetWindowRect(&rc1);
		
		m_rc.right+=rc1.left-m_rc.left;
		m_rc.bottom+=rc1.top-m_rc.top;
		m_rc.left=rc1.left;
		m_rc.top=rc1.top;

		MoveWindow(&m_rc,TRUE);
		bShow=!bShow;
		GetDlgItem(IDC_LOGSHOW)->SetWindowText(_T("日志<<"));
	}
}

void CLuzj_ZTEDlg::OnSelchangeUsername() 
{
	CString str;
	m_ccb_username.GetLBText(m_ccb_username.GetCurSel(), str);	
	GetDlgItem(IDC_PWD)->SetWindowText(Config.m_UserInfo[str]);	
}

void CLuzj_ZTEDlg::OnSetting() 
{
	CSettingDlg dlg;
	dlg.parent = this;
	//dlg.SetParent(this);	
	dlg.DoModal();
}

int CLuzj_ZTEDlg::TestAdapter(const char *name)
{
	//寻找所选的网卡的MAC
	char errbuf[PCAP_ERRBUF_SIZE];   /* error buffer */
	u_char mac[6];	
	if (GetMacIP(name, NULL, mac) != 0) return -1;
	//////////////////////////////////////////////////////////////////////////
	// 打开指定适配器
	pcap_t *handle=pcap_open_live(ToNPFName(name),65536,1,Config.m_iTimeout, errbuf);
    if(handle == NULL) {
		Log(I_INFO, "pcap_open_live:%s", errbuf);
		return -2;
	}

	char	FilterStr[100];		//包过滤字符串
	struct bpf_program	mfcode;	

	sprintf(FilterStr, "(ether proto 0x888e) and (ether dst host %02x:%02x:%02x:%02x:%02x:%02x)",
			mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	int retcode;
    if((retcode=pcap_compile(handle, &mfcode, FilterStr, 1, 0xff))==-1||(retcode=pcap_setfilter(handle, &mfcode))==-1)
	{		
		pcap_close(handle); 
		return -3;
    }
	
	CPacket packet;	
	//////////////////////////////////////////////////////////////////////////
	///开始认证包
    if(!(retcode=packet.send_packet_start(handle,mac))) {
		pcap_close(handle); return -4;
	}
	const u_char *captured;
	struct pcap_pkthdr      *header;
	char m_errorBuffer[ PCAP_ERRBUF_SIZE ];		//错误信息缓冲区
	retcode = pcap_next_ex(handle, &header, &captured);
	if (retcode != 1 && retcode != 0)
	{
		strncpy(m_errorBuffer, pcap_strerror(retcode), PCAP_ERRBUF_SIZE);
		Log(I_INFO, "网卡错误信息：%s", m_errorBuffer);
		pcap_close(handle);
		return -5;
	}
	pcap_close(handle); 
	return 0;
}

void CLuzj_ZTEDlg::OnHttpHeart() 
{
	// TODO: Add your control notification handler code here

	if(m_HttpHeartThread) {
		Log(I_MSG, "停止网页心跳，一段时间内没有数据包传送网页认证将会自动退出");
		TerminateThread(m_HttpHeartThread, 0); m_HttpHeartThread = NULL;		
		//GetDlgItem(IDC_HTTP_HEART)->SetWindowText(_T("开始网页心跳"));
	} else {
		Log(I_MSG, "开始网页心跳");
		m_HttpHeartThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)http_heart_thread, this, 0, NULL);		
		//GetDlgItem(IDC_WEB_AUTH)->EnableWindow(FALSE);
		//GetDlgItem(IDC_HTTP_HEART)->SetWindowText(_T("停止网页心跳"));
	}
}

void CLuzj_ZTEDlg::StartWebAuth()
{

	if(m_WebAuthThread) {
		TerminateThread(m_WebAuthThread, 0); m_WebAuthThread = NULL;			
		Log(I_MSG, "停止网页认证");
	}
	m_WebAuthThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)web_auth_thread, this, 0, NULL);
	
	Log(I_MSG, "开始网页认证尝试");

}

DWORD CLuzj_ZTEDlg::http_heart_thread(void *para)
{
	CLuzj_ZTEDlg* Dlg = (CLuzj_ZTEDlg*)para;
	while (Dlg->status != OFFLINE && Dlg->status != INIT) {
		DownLoadFileToBuffer(NULL, 0, Config.m_csHeartUrl, NULL, Config.m_iTimeout * 1000, Config.m_csHeartCookies);
		Dlg->logmsgsend();	
		Sleep(Config.m_iHeartInterval * 1000);
	}
	Dlg->Log(I_MSG, "网页心跳退出");
	Dlg->m_HttpHeartThread = NULL;
	//Dlg->GetDlgItem(IDC_HTTP_HEART)->SetWindowText(_T("开始网页心跳"));
	return 0;
}

DWORD CLuzj_ZTEDlg::web_auth_thread(void *para)
{
	CLuzj_ZTEDlg* Dlg = (CLuzj_ZTEDlg*)para;
	
	char *msg;
	while (Dlg->status != OFFLINE) {
		if(Dlg->GetMacIP(Config.m_csNetCard, Dlg->m_ip, NULL) == 0) {
			msg = Dlg->HttpAuth(FALSE);
			if (msg == NULL) {
				Dlg->Log(I_MSG, "网页认证成功.");
				Dlg->Log(I_INFO, "网页认证尝试已停止");
				//Dlg->ShowWindow(SW_HIDE);
				Dlg->status = HTTPED;
				if (Config.m_bHttpHeart) Dlg->OnHttpHeart();
				break;
			} else {	
				Dlg->Log(I_MSG, "网页认证错误，错误信息:%s", msg);
				WebLogout(Dlg->m_ip, Config.m_csWebLogoutUrl, Config.m_iTimeout); 
			}
		}
		Sleep(5000);
	}
	if (msg != NULL)
	{
		Dlg->WebFaultMsg(msg);
	}
	if (Dlg->status == ONLINE || Dlg->status == HTTPED)
	{
		if (Dlg->ShareServiceStatus)
		{
			if (int ErrorMSG = Dlg->StartService(_T("SharedAccess")) == SERVICE_RUNNING)
			{
				Dlg->Log(I_MSG, "重新启用ICS服务，恢复Wifi共享");
				Dlg->ShareServiceStatus = 0;
			}
			else if (ErrorMSG == 0)
				Dlg->Log(I_MSG, "ICS服务已启用");
			else
				Dlg->Log(I_MSG, "ICS服务重启失败，Wifi共享功能未能恢复，请手动重启Wifi,错误代码:%d", ErrorMSG);
		}
		Dlg->m_NetStatus = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetStatus, Dlg, 0, 0);
		Dlg->Log(I_MSG, "已可以正常上网");
		Config.m_iReauthlog = 1;
		msg = Dlg->Notice();
		if (msg){
			Dlg->Log(I_MSG, "%s", msg);
			free(msg);
		}
		if (Config.m_bAutoUpdate = 1)
		{
			Dlg->CheckUpdate();
			Dlg->CheckWinPcapUpdate();
		}
	}
	Dlg->m_WebAuthThread = NULL;
	
	return 0;
}


void CLuzj_ZTEDlg::WebAuthButton()
{
	if (m_bAuth == FALSE && (status != HTTPED || status != ONLINE))
	{
		//使得开始按钮有效，而断开按钮无效
		UpdateStatus(TRUE);

		//////////////////////////////////////////////////////////////////////////
		//		先存放设置参数
		CString strTemp;

		//取得用户名密码
		GetDlgItem(IDC_USERNAME)->GetWindowText((char*)m_username, sizeof(m_username));
		m_usernameLen = strlen((char*)m_username);
		GetDlgItem(IDC_PWD)->GetWindowText((char*)m_password, sizeof(m_password));
		m_passwordLen = strlen((char*)m_password);

		if (m_usernameLen < 1 || m_passwordLen < 1)	{
			this->Log(I_ERR, "用户名或者密码太短!"); UpdateStatus(FALSE); return;
		}

		Config.m_csNetCard = m_csAdapters[((CComboBox*)GetDlgItem(IDC_NETCARD))->GetCurSel()];

		if (IsDlgButtonChecked(IDC_REMEMBER)) {
			Config.m_bRememberPWD = TRUE;
		}
		else {
			Config.m_bRememberPWD = FALSE;
		}

		Config.m_csLastUser = (char*)m_username;
		if (Config.m_bRememberPWD)
			Config.m_UserInfo[(char*)m_username] = (char*)m_password;

		Config.SaveConfig();
		//////////////////////////////////////////////////////////////////////////
		SetBubble("提示", STR_AppName" 开始联网认证……");

		char m_errorBuffer[PCAP_ERRBUF_SIZE];
		int retcode = 0;

		/////////////////////////////////////////////////////////////////////////
		//寻找所选的网卡的MAC	
		if (GetMacIP(Config.m_csNetCard, NULL, m_MacAdd) != 0)	{
			if (m_MacAdd[0] == 0 && m_MacAdd[1] == 0 && m_MacAdd[2] == 0 &&
				m_MacAdd[3] == 0 && m_MacAdd[4] == 0 && m_MacAdd[5] == 0) {
				Log(I_ERR, "GetMacIP:no mac address."); UpdateStatus(FALSE);
				return;
			}
		}
		else {
			Log(I_INFO, "MAC:%02X-%02X-%02X-%02X-%02X-%02X", m_MacAdd[0], m_MacAdd[1], m_MacAdd[2],
				m_MacAdd[3], m_MacAdd[4], m_MacAdd[5]);
		}
		//////////////////////////////////////////////////////////////////////////
		// 打开指定适配器
		m_adapterHandle = pcap_open_live(ToNPFName(Config.m_csNetCard), 65536, 1, Config.m_iTimeout, m_errorBuffer);
		if (m_adapterHandle == NULL) {
			Log(I_INFO, "pcap_open_live:%s", m_errorBuffer); UpdateStatus(FALSE);
			return;
		}

		char	FilterStr[100];		//包过滤字符串
		struct bpf_program	mfcode;

		sprintf(FilterStr, "(ether proto 0x888e) and (ether dst host %02x:%02x:%02x:%02x:%02x:%02x)",
			m_MacAdd[0], m_MacAdd[1], m_MacAdd[2], m_MacAdd[3], m_MacAdd[4], m_MacAdd[5]);

		if ((retcode = pcap_compile(m_adapterHandle, &mfcode, FilterStr, 1, 0xff)) == -1
			|| (retcode = pcap_setfilter(m_adapterHandle, &mfcode)) == -1)
		{
			Log(I_INFO, "pcap_compile & pcap_setfilter:%s", pcap_strerror(retcode));
			pcap_close(m_adapterHandle); UpdateStatus(FALSE);
			return;
		}

		CPacket packet;
		//////////////////////////////////////////////////////////////////////////
		///开始认证包
		if (!(retcode = packet.send_packet_start(m_adapterHandle, m_MacAdd))) {
			Log(I_INFO, "send_packet_start:(%d)%s", retcode, pcap_geterr(m_adapterHandle));	 UpdateStatus(FALSE);
			return;
		}
	}
	if ((Config.m_csWebAuthUrl.GetLength() > 0)){
		status = HTTPING;
		::TerminateThread(this->m_HttpHeartThread, 0); this->m_HttpHeartThread = NULL;
		StartWebAuth();
	}
}

void CLuzj_ZTEDlg::WebAuthButton2()
{/*
	if (Config.m_bWebLogout && strlen(Config.m_csWebLogoutUrl) > 0) {
		char *msg = WebLogout(this->m_ip, Config.m_csWebLogoutUrl, Config.m_iTimeout);
		if (strcmp(msg, "logoutsuccess")==0)
		{
			this->Log(I_MSG, "网页认证已下线!");
		}
		else if (strcmp(msg, "logoutfailed")==0)
		{ 
			this->Log(I_MSG, "网页认证下线失败"); 
		}
		else
		{
			this->Log(I_MSG, "网页认证下线异常，错误信息:%s", msg);
		}
		//强制结束
		m_WebAuthThread = NULL;
		::TerminateThread(this->m_HttpHeartThread, 0); this->m_HttpHeartThread = NULL;
		Log(I_MSG, "网页心跳已停止");
		GetDlgItem(IDC_HTTP_HEART)->SetWindowText("开始网页心跳");
		this->status = AUTHED;
	}
	else
	{
		Log(I_MSG, "没有启用网页认证注销,请在设置勾选！");
	}
	if (m_bAuth)
	UpdateStatus(FALSE);*/
}


void CLuzj_ZTEDlg::LogShow()
{
	RECT rc1 = { 0 }, rc2 = { 0 };
	GetWindowRect(&rc1);

	m_rc.right += rc1.left - m_rc.left;
	m_rc.bottom += rc1.top - m_rc.top;
	m_rc.left = rc1.left;
	m_rc.top = rc1.top;

	MoveWindow(&m_rc, TRUE);
	GetDlgItem(IDC_LOGSHOW)->SetWindowText(_T("日志<<"));
	ShowWindow(SW_SHOWNA);
}
int DHCP_Auto_Restart()
{
	char command[150];
	HKEY hKey;
	char RegEnableDHCP2[300];
	TCHAR sz[20];
	DWORD dwSize = sizeof(sz);
	HINSTANCE ERRORMSG;
	for (int k = 1, j = 1; k < 003; k++, j++)
	{
		sprintf(RegEnableDHCP2, "SYSTEM\\ControlSet00%d\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\%s\\Connection", k, Config.m_csNetCard);
		LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			RegEnableDHCP2,
			NULL, KEY_ALL_ACCESS, &hKey);

		if (RegQueryValueEx(hKey, _T("Name"), NULL, NULL, (LPBYTE)sz, &dwSize) == ERROR_SUCCESS)
		{
			TCHAR sys32[MAX_PATH];
			GetSystemDirectory(sys32, sizeof(sys32));
			sprintf(sys32, "%s\\cmd.exe", sys32);
			sprintf(command, "/c netsh interface ip set address name=\"%s\" source=dhcp", sz);
			ERRORMSG=ShellExecute(NULL, _T("open"), sys32, command, _T(""), SW_HIDE);
			sprintf(command, "/c netsh interface set interface name=\"%s\" admin=DISABLED", sz);
			ERRORMSG=ShellExecute(NULL, _T("open"), sys32, command, _T(""), SW_HIDE);
			if ((int)ERRORMSG > 32)
			{
				Sleep(10000);
				sprintf(command, "/c netsh interface set interface name=\"%s\" admin=ENABLED", sz);
				ERRORMSG = ShellExecute(NULL, _T("open"), sys32, command, _T(""), SW_HIDE);
				if ((int)ERRORMSG > 32)Sleep(6000);
			}
			RegCloseKey(hKey);
			return 0;
		}
	}
	RegCloseKey(hKey);
	return 1;
}
int CLuzj_ZTEDlg::DHCP_Auto()
{
	Config.m_csNetCard = m_csAdapters[((CComboBox*)GetDlgItem(IDC_NETCARD))->GetCurSel()];

	int Sign = 0;
	//自动更改为自动获取ip
	if (Config.m_bDHCP)
	{
		HKEY DHCPIP;
		char RegEnableDHCP[150];
		DWORD regsz = REG_DWORD;
		DWORD pjPath=1;
		DWORD iPathLen;

		for (int k = 1, j = 1; k < 003; k++, j++)
		{
			sprintf(RegEnableDHCP, "SYSTEM\\ControlSet00%d\\services\\Tcpip\\Parameters\\Interfaces\\%s", k, Config.m_csNetCard);
			LONG errCode = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
				RegEnableDHCP,
				NULL, KEY_ALL_ACCESS, &DHCPIP);
			iPathLen = sizeof(DWORD);
			errCode = ::RegQueryValueEx(DHCPIP, _T("EnableDHCP"), NULL, &regsz, (unsigned char*)&pjPath, &iPathLen);
			if (errCode == ERROR_SUCCESS && pjPath != 1)
			{
				pjPath = 1;
				errCode = ::RegSetValueEx(DHCPIP, _T("EnableDHCP"), NULL, REG_DWORD, (unsigned char*)&pjPath, sizeof(DWORD));
				if (errCode == ERROR_SUCCESS)
				{
					Log(I_MSG, "设置网卡已自动设置为自动获取ip");
					Sign = 1;
				}
				else
				{
					Log(I_ERR, "设置网卡已自动设置为自动获取ip失败，请手动设置！");
					RegCloseKey(DHCPIP);
					return 0;
				}
			}
		}
		RegCloseKey(DHCPIP);
		if (Sign)
		{
			ShowWindow(SW_HIDE);
			SetBubble("提示", STR_AppName" 正在重启网卡中……");
			if (DHCP_Auto_Restart()){
				Log(I_ERR, "自动重启网卡失败，请手动重启网卡或拔出网线重新插入！");
				return 0;
			}
			else
			{
				Log(I_MSG, "自动重启网卡成功,自动重新认证！");
			}
		}
	}
	return 1;
}

void CLuzj_ZTEDlg::OnBnClickedFeedback()
{
	/*
	// TODO:  在此添加控件通知处理程序代码
	Feedback dlg;
	dlg.parent = this;
	//dlg.SetParent(this);	
	dlg.DoModal();
	*/
	::ShellExecute(this->GetSafeHwnd(), _T("open"), "http://bbs.gxgk.cc/forum.php?mod=viewthread&tid=7203", NULL, NULL, SW_SHOWNORMAL);
}
//响应ping的过程中ping线程发过来的状态消息
LRESULT CLuzj_ZTEDlg::OnStatus(WPARAM wParam, LPARAM lParam)
{
	char* pMsg = (char*)lParam;
	static int overtime=0;
	Config.m_Reauth = 0;
	if ((strcmp(pMsg, "Ping超时") == 0) && Config.m_Reauth)
	{
		if (overtime < 10)
			overtime++;
		else
		{
			m_pingThread.Pingstop = 1;
			overtime = 0;
			SendMessage(WM_COMMAND, MAKEWPARAM(IDC_LOGOFF, 0));
			SendMessage(WM_COMMAND, MAKEWPARAM(IDC_START, 0));
			delete pMsg;
			return 0;
		}
	}
	m_StatusBar.SetText(pMsg, 4, 0);
	
	delete pMsg;

	return 0;
}
//ping结束
LRESULT CLuzj_ZTEDlg::OnPingEnd(WPARAM wParam, LPARAM lParam)
{
	if (lParam == 1)
	{
		m_StatusBar.SetText("", 4, 0);
	}
	return 0;
}
