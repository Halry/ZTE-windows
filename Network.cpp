
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

	/* 检查命令行参数的合法性 */


	/* 打开输出适配器 */
	if ((fp = pcap_open_live(ToNPFName(Config.m_csNetCard), 100, 1, 1000, errbuf)) == NULL)
	{
		Dlg->m_StatusBar.SetText(_T("\nUnable to open adapter %s."), 3, 0);
		
		return 0;
	}

	/* 不用关心掩码，在这个过滤器中，它不会被使用 */
	//netmask = 0xffffff;

	// 编译过滤器
	//if (pcap_compile(fp, &fcode, "tcp"/*我用的是ADSL这里是设置只接收PPPOE的包*/, 1, netmask) <0)
	//{
	//	fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
		/* 释放设备列表 */
	//	return 0;
	//}

	//设置过滤器
	//if (pcap_setfilter(fp, &fcode)<0)
	//{
	//	fprintf(stderr, "\nError setting the filter.\n");
	//	pcap_close(fp);
		/* 释放设备列表 */
	//	return 0;
	//}

	/* 将接口设置为统计模式 */
	if (pcap_setmode(fp, MODE_STAT)<0)
	{
		Dlg->m_StatusBar.SetText(_T("\nError setting the mode."), 3, 0);
		pcap_close(fp);
		/* 释放设备列表 */
		return 0;
	}

	/* 开始主循环 */
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

	/* 以毫秒计算上一次采样的延迟时间 */
	/* 这个值通过采样到的时间戳获得 */
	delay = (header->ts.tv_sec - Dlg->tv_sec) * 1000000 - Dlg->tv_usec + header->ts.tv_usec;
	/* 获取每秒的比特数b/s */
	Bps.QuadPart = (((*(LONGLONG*)(pkt_data + 8)) * 8 * 1000000) / (delay));
	/*                                            ^      ^
	|      |
	|      |
	|      |
	将字节转换成比特 --   |
	|
	延时是以毫秒表示的 --
	*/

	/* 得到每秒的数据包数量 */
	Pps.QuadPart = (((*(LONGLONG*)(pkt_data)) * 1000000) / (delay));

	/* 将时间戳转化为可识别的格式 */
	local_tv_sec = header->ts.tv_sec;
	//ltime = localtime(&local_tv_sec);
	//strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* 打印时间戳*/
	sprintf(logmsg,"网络流量：KBPS=%.1lf PPS=%I64u", (float)Bps.QuadPart/8192, Pps.QuadPart);
	Dlg->Flux = (float)Bps.QuadPart / 8192;
	Dlg->m_StatusBar.SetText(_T(logmsg), 3, 0);
	/* 打印采样结果 */
	//printf("BPS=%I64u ", );
	//printf("\n", );
	
	//存储当前的时间戳
	Dlg->tv_sec = header->ts.tv_sec;
	Dlg->tv_usec = header->ts.tv_usec;
}
