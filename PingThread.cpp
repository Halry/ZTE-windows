// PingThread.cpp: implementation of the CPingThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PingThread.h"
#include <process.h>    /* _beginthread, _endthread */

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPingThread::CPingThread()
{
	m_dwID = 0;  
	m_hThread = NULL;
	//创建信号事件
	m_hKillEvent =   CreateEvent(NULL,TRUE,FALSE,NULL);
	m_hSignalEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	//开是一个ping线程
	m_hThread = (HANDLE) _beginthreadex(NULL,						
										0,							
										ThreadProc,     			
										(void*) this,	      
										0,							
										&m_dwID);					


}

CPingThread::~CPingThread()
{
	SetEvent(m_hKillEvent);
	WaitForSingleObject(m_hThread,INFINITE);

}

//ping线程过程函数
UINT CPingThread::ThreadProc(void* lpParam)
{
	CPingThread* pThis = reinterpret_cast<CPingThread*>(lpParam);
	 pThis->Pingstopsign=&pThis->Pingstop;
	while (1)
	{
		HANDLE hObjects[2];
		hObjects[0] = pThis->m_hKillEvent;
		hObjects[1] = pThis->m_hSignalEvent;

		//等待信号有效
		DWORD dwWait = WaitForMultipleObjects(2,hObjects,FALSE,INFINITE);
		if (dwWait == WAIT_OBJECT_0)		
			break;

		//开始ping
		if (dwWait == WAIT_OBJECT_0 + 1)
			pThis->m_ping.Ping(pThis->m_nRetries, pThis->m_strHost, pThis->m_hWnd, pThis->Pingstopsign);
	}
	return 0;
}

//开始ping
void CPingThread::StartPing(UINT nRetries,CString strHost, HWND hWnd)
{
	m_nRetries = nRetries;
	m_strHost = strHost;
	m_hWnd = hWnd;

	//设置信号有效，可以开始线程了
	SetEvent(m_hSignalEvent);
}