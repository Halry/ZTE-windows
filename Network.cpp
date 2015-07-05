
#include "stdafx.h"
#include "Luzj_ZTEDlg.h"
#define MAX_BUFFER_SIZE 10204
static char logmsg[MAX_BUFFER_SIZE];

DWORD CLuzj_ZTEDlg::NetStatus(void *para)
{
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg*)para;
	pcap_t *fp;
	char errbuf[PCAP_ERRBUF_SIZE];
	//struct timeval st_ts;
	//u_int netmask;
	//struct bpf_program fcode;

	/* ��������в����ĺϷ��� */


	/* ����������� */
	if ((fp = pcap_open_live(ToNPFName(Config.m_csNetCard), 100, 1, 1000, errbuf)) == NULL)
	{
		Dlg->m_StatusBar.SetText(_T("\nUnable to open adapter %s."), 3, 0);
		
		return 0;
	}

	/* ���ù������룬������������У������ᱻʹ�� */
	//netmask = 0xffffff;

	// ���������
	//if (pcap_compile(fp, &fcode, "tcp"/*���õ���ADSL����������ֻ����PPPOE�İ�*/, 1, netmask) <0)
	//{
	//	fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* �ͷ��豸�б� */
	//	return 0;
	//}

	//���ù�����
	//if (pcap_setfilter(fp, &fcode)<0)
	//{
	//	fprintf(stderr, "\nError setting the filter.\n");
	//	pcap_close(fp);
		/* �ͷ��豸�б� */
	//	return 0;
	//}

	/* ���ӿ�����Ϊͳ��ģʽ */
	if (pcap_setmode(fp, MODE_STAT)<0)
	{
		Dlg->m_StatusBar.SetText(_T("\nError setting the mode."), 3, 0);
		pcap_close(fp);
		/* �ͷ��豸�б� */
		return 0;
	}

	/* ��ʼ��ѭ�� */
	pcap_loop(fp, 0, CLuzj_ZTEDlg::dispatcher_handler, (u_char*)Dlg);

	pcap_close(fp);
	return 0;
}

void CLuzj_ZTEDlg::dispatcher_handler(u_char *state, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	CLuzj_ZTEDlg *Dlg = (CLuzj_ZTEDlg*)state;
	//struct timeval *old_ts = (struct timeval *)state;
	u_int delay;
	LARGE_INTEGER Bps, Pps;
	//struct tm *ltime;
	//char timestr[16];
	time_t local_tv_sec;

	/* �Ժ��������һ�β������ӳ�ʱ�� */
	/* ���ֵͨ����������ʱ������ */
	delay = (header->ts.tv_sec - Dlg->tv_sec) * 1000000 - Dlg->tv_usec + header->ts.tv_usec;
	/* ��ȡÿ��ı�����b/s */
	Bps.QuadPart = (((*(LONGLONG*)(pkt_data + 8)) * 8 * 1000000) / (delay));
	/*                                            ^      ^
	|      |
	|      |
	|      |
	���ֽ�ת���ɱ��� --   |
	|
	��ʱ���Ժ����ʾ�� --
	*/

	/* �õ�ÿ������ݰ����� */
	Pps.QuadPart = (((*(LONGLONG*)(pkt_data)) * 1000000) / (delay));

	/* ��ʱ���ת��Ϊ��ʶ��ĸ�ʽ */
	local_tv_sec = header->ts.tv_sec;
	//ltime = localtime(&local_tv_sec);
	//strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* ��ӡʱ���*/
	sprintf(logmsg,"����������KBPS=%.1lf PPS=%I64u", (float)Bps.QuadPart/8192, Pps.QuadPart);
	Dlg->Flux = (float)Bps.QuadPart / 8192;
	Dlg->m_StatusBar.SetText(_T(logmsg), 3, 0);
	/* ��ӡ������� */
	//printf("BPS=%I64u ", );
	//printf("\n", );
	
	//�洢��ǰ��ʱ���
	Dlg->tv_sec = header->ts.tv_sec;
	Dlg->tv_usec = header->ts.tv_usec;
}
