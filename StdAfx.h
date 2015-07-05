// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently

#if !defined(AFX_STDAFX_H__7C0E3A18_9A51_445F_8810_EFC647794D15__INCLUDED_)
#define AFX_STDAFX_H__7C0E3A18_9A51_445F_8810_EFC647794D15__INCLUDED_
#define  _CRT_SECURE_NO_WARNINGS

#ifndef _WIN32_IE // 
#define _WIN32_IE 0x0500 //
#endif

#if _MSC_VER > 1000
#pragma once
#define NO_WARN_MBCS_MFC_DEPRECATION
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#pragma comment(lib, "libcurl_imp.lib")
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <windows.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Wldap32.lib")//curl依赖库
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib")

#include "pcap.h"
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "version.lib")//版本获取函数

//#pragma warning (disable: 4800)
#define CURL_STATICLIB
#include "curl/curl.h"
#ifdef CURL_STATICLIB
#ifdef _DEBUG
#pragma comment(lib, "libcurl2010.lib")
#else
#pragma comment(lib, "libcurl2010.lib")
//#pragma comment(lib, "libcurld_static.lib")//VS2013生成，不支持XP
#endif
#else
#pragma comment(lib, "libcurl.lib")
#endif
//#pragma warning (default: 4800)

#include "WebAuth.h"

typedef  unsigned char	u_char;

//ping程序
#define WM_MSG_STATUS	WM_USER + 0x0100
#define WM_PING_END		WM_USER + 0x0101
static char* AllocBuffer(CString strMsg)
{
	int nLen = strMsg.GetLength();
	char *pBuffer = new char[nLen + 1];

	strcpy(pBuffer, (const char*)strMsg);

	ASSERT(pBuffer != NULL);
	return pBuffer;
}
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7C0E3A18_9A51_445F_8810_EFC647794D15__INCLUDED_)

#include <comdef.h>//初始化一下com口
#include <afxdisp.h>
