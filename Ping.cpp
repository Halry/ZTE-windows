
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

	//����һ��Raw�׽���
	rawSocket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (rawSocket == SOCKET_ERROR)
	{
		WSAError("socket()");
		return;
	}

	//���������Ϣ
	lpHost = gethostbyname(pstrHost);
	if (lpHost == NULL)
	{
		str.Format("Host not found: %s", pstrHost);
		::PostMessage(m_hWnd, WM_MSG_STATUS, 0, (LPARAM)AllocBuffer(str));
		::PostMessage(m_hWnd, WM_PING_END, 0, 0);
		return;
	}

	//����Ŀ���׽��ֵ�ַ��Ϣ
	saDest.sin_addr.s_addr = *((u_long FAR *) (lpHost->h_addr));
	saDest.sin_family = AF_INET;
	saDest.sin_port = 3077;//0;

	//�����û��������ڵĹ���
	str.Format("Pinging %s [%s] with %d bytes of data:",
		pstrHost,
		inet_ntoa(saDest.sin_addr),
		REQ_DATASIZE);
	::PostMessage(m_hWnd, WM_MSG_STATUS, 1, (LPARAM)AllocBuffer(str));

	//���ping
	for (;; /* nLoop++ */)
	{
		//����ICMP��Ӧ����
		SendEchoRequest(rawSocket, &saDest);

		nRet = WaitForEchoReply(rawSocket);
		if (nRet == SOCKET_ERROR)
		{
			WSAError("select()");
			break;
		}
		if (!nRet)
		{
			str.Format("Ping��ʱ");
			::PostMessage(m_hWnd, WM_MSG_STATUS, 3, (LPARAM)AllocBuffer(str));
		}
		else

		{
			//��û�Ӧ
			dwTimeSent = RecvEchoReply(rawSocket, &saSrc, &cTTL);

			//����ʱ��
			dwElapsed = GetTickCount() - dwTimeSent;
			str.Format("�����ӳ�:%ldms",
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

//����ICMPECHO���ݰ�����
int CPing::SendEchoRequest(SOCKET s, LPSOCKADDR_IN lpstToAddr)
{
	static ECHOREQUEST echoReq;
	static int nId = 1;
	static int  nSeq = 1;
	int nRet;

	//�����Ӧ����
	echoReq.icmpHdr.Type = ICMP_ECHOREQ;
	echoReq.icmpHdr.Code = 0;
	echoReq.icmpHdr.Checksum = 0;
	echoReq.icmpHdr.ID = nId++;
	echoReq.icmpHdr.Seq = nSeq++;

	for (nRet = 0; nRet < REQ_DATASIZE; nRet++)
		echoReq.cData[nRet] = ' ' + nRet;

	//���淢��ʱ��
	echoReq.dwTime = GetTickCount();

	echoReq.icmpHdr.Checksum = in_cksum((u_short *)&echoReq, sizeof(ECHOREQUEST));

	//��������
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

//����ICMPECHO���ݰ���Ӧ
DWORD CPing::RecvEchoReply(SOCKET s, LPSOCKADDR_IN lpsaFrom, u_char *pTTL)
{
	ECHOREPLY echoReply;
	int nRet;
	int nAddrLen = sizeof(struct sockaddr_in);

	//���������Ӧ
	nRet = recvfrom(s,
		(LPSTR)&echoReply,
		sizeof(ECHOREPLY),
		0,
		(LPSOCKADDR)lpsaFrom,
		&nAddrLen);

	//��鷵��ֵ
	if (nRet == SOCKET_ERROR)
		WSAError("recvfrom()");

	//���ط��͵�ʱ��
	*pTTL = echoReply.ipHdr.TTL;

	return(echoReply.echoRequest.dwTime);
}

//�ȴ���Ӧ
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

//������
void CPing::WSAError(LPCSTR lpMsg)
{
	CString strMsg;
	strMsg.Format("%s - WSAError: %ld", lpMsg, WSAGetLastError());
	//���ͱ�����Ϣ
	::PostMessage(m_hWnd, WM_MSG_STATUS, 0, (LPARAM)AllocBuffer(strMsg));
}

//ת����ַ
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
