#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// Feedback dialog
#include "Config.h"
#include "HyperLink.h"
#include "Resource.h"
#include "CWebbrowser2.h"
#include <MsHTML.h>
#include <atlbase.h>
#include <comdef.h>
extern CConfig Config;
class Feedback :public CDialog
{
public:
	Feedback(CWnd* pParent = NULL);
	enum { IDD = IDD_Feedback };
	CWnd *parent;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CWebBrowser2 m_web;
};

