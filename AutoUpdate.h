#ifndef AUTOUPDATE_H
#define AUTOUPDATE_H

#define WEB_URL    TEXT("http://gklogin.lastorder.cn/gklogin")
#ifdef _DEBUG
#define LAN_URL  TEXT("http://gxgk.lastorder.cn/gklogintest")
#else
#define LAN_URL  TEXT("http://gxgk.lastorder.cn/gklogintest")
#endif
#define URL_ZTE      TEXT("/ZTE.exe")
#define URL_WinPcap  TEXT("/WinPcap.exe")
#define NoticeUrl    TEXT("/Notice.html")

#define MAX_STRING 2048

typedef struct _FileInfo{
	time_t modifytime;
	size_t filesize;
	char Version[MAX_STRING];
	char WinPcapVer[MAX_STRING];
} FileInfo;

typedef struct _buffer{
	char *buffer;
	size_t size;
	int offset;

} Buffer;

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
int AutoUpdate();
int AutoUpdateWinPcap();
FileInfo *GetFileModifyTime();
FileInfo *GetHttpModifyTime(const char *url);
char *RandomSessionID(const char *url);
int DownLoadFile(const char *savename, const char *url, const char *ip, const int timeout);
int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies);
int SendPost(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies, const char *data);
int VersionUpdate(FileInfo *fileinfo);
#endif