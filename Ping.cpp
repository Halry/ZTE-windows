
#include "stdafx.h"
#include "ping.h"


void CPing::Ping(UINT nRetries, LPCSTR pstrHost, HWND hWnd, bool *Pingstopsign)
{
	SOCKET	  rawSocket;
	LPHOSTENT lpHost;
	int       nRet;
	struct    sockaddr_in saDest;
	struct    sockaddr_in saSrc;
	DWORD	  dwTimeSent;
	DWORD	  dwElapsed;
	u_char    cTTL;


	m_hWnd = hWnd;

	CString str;

	ASSERT(IsWindow(hWnd));

	//创建一个Raw套节字
	rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (rawSocket == SOCKET_ERROR)
	{
		WSAError("socket()");
		return;
	}

	//获得主机信息
	lpHost = gethostbyname(pstrHost);
	if (lpHost == NULL)
	{
		str.Format("Host not found: %s", pstrHost);
		::PostMessage(m_hWnd, WM_MSG_STATUS, 0, (LPARAM)AllocBuffer(str));
		::PostMessage(m_hWnd, WM_PING_END, 0, 0);
		return;
	}

	//构造目标套节字地址信息
	saDest.sin_addr.s_addr = *((u_long FAR *) (lpHost->h_addr));
	saDest.sin_family = AF_INET;
	saDest.sin_port = 3077;//0;

	//告诉用户我们现在的工作
	str.Format("Pinging %s [%s] with %d bytes of data:",
		pstrHost,
		inet_ntoa(saDest.sin_addr),
		REQ_DATASIZE);
	::PostMessage(m_hWnd, WM_MSG_STATUS, 1, (LPARAM)AllocBuffer(str));

	//多次ping
	for (;; /* nLoop++ */)
	{
		//发送ICMP回应请求
		SendEchoRequest(rawSocket, &saDest);

		nRet = WaitForEchoReply(rawSocket);
		if (nRet == SOCKET_ERROR)
		{
			WSAError("select()");
			break;
		}
		if (!nRet)
		{
			str.Format("Ping超时");
			::PostMessage(m_hWnd, WM_MSG_STATUS, 3, (LPARAM)AllocBuffer(str));
		}
		else

		{
			//获得回应
			dwTimeSent = RecvEchoReply(rawSocket, &saSrc, &cTTL);

			//计算时间
			dwElapsed = GetTickCount() - dwTimeSent;
			str.Format("内网延迟:%ldms",
				//nLoop + 1,
				//inet_ntoa(saSrc.sin_addr),
				//REQ_DATASIZE,
				dwElapsed
				//,cTTL
				);

			::PostMessage(m_hWnd, WM_MSG_STATUS, 2, (LPARAM)AllocBuffer(str));

			Sleep(1000);

		}
		//*******************
		if (*Pingstopsign)
			break;
		//********************************

	}

	::PostMessage(m_hWnd, WM_PING_END, 0, 1);

	nRet = closesocket(rawSocket);
	if (nRet == SOCKET_ERROR)
		WSAError("closesocket()");

}

//发送ICMPECHO数据包请求
int CPing::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr)
{
	static ECHOREQUEST echoReq;
	static int nId = 1;
	static int  nSeq = 1;
	int nRet;

	//构造回应请求
	echoReq.icmpHdr.Type = ICMP_ECHOREQ;
	echoReq.icmpHdr.Code = 0;
	echoReq.icmpHdr.Checksum = 0;
	echoReq.icmpHdr.ID = nId++;
	echoReq.icmpHdr.Seq = nSeq++;

	for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' ' + nRet;

	//保存发送时间
	echoReq.dwTime = GetTickCount();

	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));

	//发送请求
	nRet = sendto(s,
		(LPSTR)&echoReq,
		sizeof(ECHOREQUEST),
		0,
		(LPSOCKADDR)lpstToAddr,
		sizeof(SOCKADDR_IN));

	if (nRet == SOCKET_ERROR)
		WSAError("sendto()");
	return (nRet);
}

//接收ICMPECHO数据包回应
DWORD CPing::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL)
{
	ECHOREPLY echoReply;
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);

	//接收请求回应
	nRet = recvfrom(s,
		(LPSTR)&echoReply,
		sizeof(ECHOREPLY),
		0,
		(LPSOCKADDR)lpsaFrom,
		&nAddrLen);

	//检查返回值
	if (nRet == SOCKET_ERROR)
		WSAError("recvfrom()");

	//返回发送的时间
	*pTTL = echoReply.ipHdr.TTL;

	return(echoReply.echoRequest.dwTime);
}

//等待回应
int CPing::WaitForEchoReply(SOCKET s)
{
	struct timeval Timeout;
	fd_set readfds;

	readfds.fd_count = 1;
	readfds.fd_array[0] = s;
	Timeout.tv_sec = 1;
	Timeout.tv_usec = 0;

	return(select(1, &readfds, NULL, NULL, &Timeout));
}

//错误处理
void CPing::WSAError(LPCSTR lpMsg)
{
	CString strMsg;
	strMsg.Format("%s - WSAError: %ld", lpMsg, WSAGetLastError());
	//发送报错信息
	::PostMessage(m_hWnd, WM_MSG_STATUS, 0, (LPARAM)AllocBuffer(strMsg));
}

//转换地址
u_short CPing::in_cksum(u_short *addr, int len)
{
	register int nleft = len;
	register u_short *w = addr;
	register u_short answer;
	register int sum = 0;

	while (nleft > 1)  {
		sum += *w++;
		nleft -= 2;
	}

	if (nleft == 1) {
		u_short	u = 0;

		*(u_char *)(&u) = *(u_char *)w;
		sum += u;
	}

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}
