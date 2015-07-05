#include "stdafx.h"
#include "Define.h"
#include "AutoUpdate.h"
#include <stdio.h>
#include "time.h"
#define FILETIME_LENGTH	64

static size_t write_file_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

static size_t write_buffer_data(void *ptr, size_t size, size_t nmemb, void *buffer)
{
	Buffer *buf = (Buffer*)buffer;
	if(buf->buffer == NULL) return size;
	if(buf->offset + size >= buf->size) return 0;
	memcpy(buf->buffer + buf->offset, ptr, size * nmemb);	
	buf->offset += size * nmemb;
	return size;
}

FileInfo *GetFileModifyTime(const char* url)
{
	static FileInfo info;
	char filepath[MAX_PATH] = {0};
	info.filesize = 0; info.modifytime = 0;

	if(!GetModuleFileName(NULL, filepath, MAX_PATH)) return NULL;

	HANDLE hFile = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, 0, NULL);
	if(hFile == NULL) return NULL;

	FILETIME LastModifyTime;

	LARGE_INTEGER largeInt;
	if(GetFileTime(hFile, NULL, NULL, &LastModifyTime)) {		
		largeInt.QuadPart =  ((DWORD64)LastModifyTime.dwHighDateTime << 32) + LastModifyTime.dwLowDateTime;
		info.modifytime = (long)((largeInt.QuadPart - 116444736000000000) /  10000000);
	}

	info.filesize = GetFileSize(hFile, NULL)/1024;
	strcpy(info.Version , STR_Version);
	CloseHandle(hFile);
	
	return &info;
}

int AutoUpdate()
{
	FileInfo *remote_info = GetHttpModifyTime(LAN_URL URL_ZTE);
	if (remote_info == NULL){
		remote_info = GetHttpModifyTime(WEB_URL URL_ZTE);
	}
	FileInfo *local_info = GetFileModifyTime(NULL);
	if (local_info == NULL || remote_info == NULL)
	{
		//AfxMessageBox("获取本地信息失败或远程服务器无法连接，检测更新失败，请手动更新！");
		return -1;
	}
	if(strcmp(local_info->Version , remote_info->Version)<0) 
	{
		char msg[1024] = "发现新版本，立即进行更新吗？\r\n\r\n";
		char szTime[1024];
		strftime(szTime, 1024, "%Y-%m-%d %H:%M:%S", localtime(&remote_info->modifytime));
		strcat(msg, "新版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		itoa(remote_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "KB\r\n");
		strcat(msg, "版本号：");
		strcat(msg, remote_info->Version); strcat(msg, "\r\n\r\n");

		strftime(szTime,1024,"%Y-%m-%d %H:%M:%S",localtime(&local_info->modifytime));	
		strcat(msg, "当前版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		itoa(local_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "KB");
		strcat(msg, "\r\n版本号：");
		strcat(msg, local_info->Version); strcat(msg, "\r\n");

		if(IDYES != AfxMessageBox(msg, MB_YESNO | MB_ICONINFORMATION)) return 1;
		
		char szTempPath[MAX_PATH], szTempName[MAX_PATH];
		GetTempPath(MAX_PATH, szTempPath);
		GetTempFileName(szTempPath, _T("ZTE"), 0, szTempName);
		strcat(szTempName, ".exe");

		if (DownLoadFile(szTempName, LAN_URL URL_ZTE, NULL, 0) != 0)
		{
			if (DownLoadFile(szTempName, WEB_URL URL_ZTE, NULL, 0) != 0)
			{
				AfxMessageBox("下载客户端失败，请手动更新！");
				return -2;
			}
		}
		if(IDOK != AfxMessageBox("更新认证客户端期间可能短暂无法使用网络，请备份好数据，然后按“确定”开始更新。",
			MB_OK | MB_ICONINFORMATION)) {
			return 2;
		}

		char filepath[MAX_PATH] = {0};
		char parameters[MAX_PATH] = "-update \"";
		if(!GetModuleFileName(NULL, filepath, MAX_PATH)) return -1;
		strcat(parameters, filepath);

		SHELLEXECUTEINFO ShExecInfo = {0};	
		ShExecInfo.cbSize	= sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile		= szTempName;	
		ShExecInfo.lpParameters = parameters;
		ShExecInfo.nShow	= SW_HIDE;
		
		if(!ShellExecuteEx(&ShExecInfo)) {
			AfxMessageBox(_T("启动更新程序失败！"));
			return 3;
		}
		Sleep(1000);
		
		return 0;
	} 
	//UpdateFile(URL_ZTE, GetFileModifyTime());
	
	return 4;
}

int AutoUpdateWinPcap()
{
	FileInfo *remote_info = GetHttpModifyTime(LAN_URL URL_WinPcap);
	if (remote_info == NULL)
	{
		remote_info = GetHttpModifyTime(WEB_URL URL_WinPcap);
		if (remote_info == NULL)
		{
			//AfxMessageBox("远程服务器无法连接，请手动更新WinPcap！");
			return -1;
		}
	}
	CString LocalWinPcapinfo = const_cast<char*>(pcap_lib_version());
	LocalWinPcapinfo = LocalWinPcapinfo.Mid(16, 5);
	if (strcmp(remote_info->WinPcapVer, LocalWinPcapinfo)>0)
	{
		char msg[1024] = "发现WinPcap新版本，立即进行更新WinPcap吗？\r\n\r\n";
		char szTime[1024];
		strftime(szTime, 1024, "%Y-%m-%d %H:%M:%S", localtime(&remote_info->modifytime));
		strcat(msg, "新版本信息:\r\n文件时间："); strcat(msg, szTime); strcat(msg, "\r\n文件大小：");
		itoa(remote_info->filesize, szTime, 10); strcat(msg, szTime); strcat(msg, "KB");
		strcat(msg, "\r\n版本号：");
		strcat(msg, remote_info->WinPcapVer); strcat(msg, "\r\n\r\n");

		strcat(msg, "当前版本信息:\r\n"); strcat(msg, "版本号：");
		strcat(msg, LocalWinPcapinfo); strcat(msg, "\r\n");
		
		if (IDYES != AfxMessageBox(msg, MB_YESNO | MB_ICONINFORMATION)) return 1;
		if (DownLoadFile(".\\WinPcap.exe", LAN_URL URL_WinPcap, NULL, 0) != 0)
		{
			if (DownLoadFile(".\\WinPcap.exe", WEB_URL URL_WinPcap, NULL, 0) != 0)
			{
				AfxMessageBox(_T("下载WinPcap失败，请手动更新！"));
				return -2;
			}
		}
		if (IDOK != AfxMessageBox(_T("更新WinPcap期间可能短暂无法使用网络，请按“确定”开始更新。"),
			MB_OK | MB_ICONINFORMATION)) {
			return 2;
		}

		SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpFile = ".\\WinPcap.exe";
		ShExecInfo.nShow = SW_HIDE;

		if (!ShellExecuteEx(&ShExecInfo)) {
			AfxMessageBox(_T("启动更新程序失败！"));
			return 3;
		}
		Sleep(1000);
		return 0;
	}
	return 4;
}
int DownLoadFile(const char *savename, const char *url, const char *ip, const int timeout)
{
	FILE *file = NULL;
	file = fopen(savename, "wb");
	if(file == NULL) return 1;

	CURL *curl;
	CURLcode res;
	long retcode = 0;
				
	curl = curl_easy_init();
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, url);		
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		if(ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		if(timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)file);
		res = curl_easy_perform(curl);
		/*
		if(res == CURLE_OK) {			
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				
			}
		}
		*/
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	fclose(file);
	file = NULL;
	return 0;
}

int DownLoadFileToBuffer(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies)
{
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	Buffer buf;

	buf.buffer = buffer;
	buf.size = size;
	buf.offset = 0;	
	
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_REFERER, url);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);			
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");	
		if(cookies)  curl_easy_setopt(curl, CURLOPT_COOKIE, cookies); 		
		if(ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);
		if(timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);
		res = curl_easy_perform(curl);		
		
		if(res != CURLE_OK && res != CURLE_WRITE_ERROR) {
			if (res == CURLE_OPERATION_TIMEDOUT)
			{
				curl_easy_cleanup(curl);
				return -1;
			}
			curl_easy_cleanup(curl);
			return -2;
		}

		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
		if(res != CURLE_OK || retcode != 200) {
			curl_easy_cleanup(curl);
			return -3;
		}
		
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	return buf.offset;
}

FileInfo *GetHttpModifyTime(const char* url)
{
	
	static FileInfo fileinfo;
	if(VersionUpdate(&fileinfo)==0)
		return NULL;
	double size;
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	fileinfo.filesize = 0; fileinfo.modifytime = 0;
			
	curl = curl_easy_init();
	if(curl) {	
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_FILETIME, 1);		
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//认证之前会自动转跳
		res = curl_easy_perform(curl);
		if(res == CURLE_OK) {	
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &fileinfo.modifytime);				
				if(res != CURLE_OK) fileinfo.modifytime = 0;
				res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &size);
				if(res != CURLE_OK) fileinfo.filesize = 0;
				else fileinfo.filesize = (int)size;
				curl_easy_cleanup(curl);
				fileinfo.filesize = fileinfo.filesize / 1024;
				return &fileinfo;
			}
		}
		/* always cleanup */ 
		curl_easy_cleanup(curl);
		
	}
	
	return NULL;
}
int VersionUpdate(FileInfo *fileinfo)
{
#define WinPcapVerSign "WinPcap:\"" 
#define VersionSign "Version:\""
	static char http[MAX_STRING] = { 0 };
	char *p = (char *)malloc(MAX_STRING), *q, *msg = (char *)malloc(MAX_STRING);
	char *x = p, *y = msg;
	int ret = DownLoadFileToBuffer(http, MAX_STRING, LAN_URL NoticeUrl, NULL, 10, NULL);
	if (ret <= 0)
	{
		int ret = DownLoadFileToBuffer(http, MAX_STRING, WEB_URL NoticeUrl, NULL, 10, NULL);
		if (ret <= 0)
		{
			return NULL;
		}
	}
	strcpy(p, http);
	for (int i = 1; i <= 2; i++)
	{
		strcpy(p, http);//strcpy会移动指针，必须重新赋值
		p = strstr(p, i == 1 ? WinPcapVerSign : VersionSign);
		if (p == NULL)
		{
			return NULL;
		}
		p += strlen(i == 1 ? WinPcapVerSign : VersionSign);
		q = strstr(p, "\"");
		if (q == NULL)
			return NULL;
		*q = '\0';
		strcpy(i == 1 ? fileinfo->WinPcapVer : fileinfo->Version,p);
	}
	if (strlen(fileinfo->Version) == 0 && strlen(fileinfo->WinPcapVer) == 0){
		return NULL;
	}
	free(x); free(y);
	return 1;
}

size_t write_data(void* buffer, size_t size, size_t nmemb, void *stream)
{
	    FILE *fptr = (FILE*)stream;
	    fwrite(buffer, size, nmemb, fptr);
	    return size*nmemb;
}

int SendPost(char *buffer, int size, const char *url, const char *ip, const int timeout, const char *cookies,const char *data)
{
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	Buffer buf;

	buf.buffer = buffer;
	buf.size = size;
	buf.offset = 0;

	//FILE* fptr;
	/*
	if ((fptr = fopen("Sendlog.log", "w")) == NULL)
	{
		fprintf(stderr, "fopen file error:%s\n", "Sendlog.log");
	}*/
	

	struct curl_slist *http_header = NULL;
	curl = curl_easy_init();

	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url); //url地址
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data); //post参数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_buffer_data);//对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&buf);//这是write_data的第四个参数值
		curl_easy_setopt(curl, CURLOPT_POST, 1); //设置问非0表示本次操作为post
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); //打印调试信息
		curl_easy_setopt(curl, CURLOPT_HEADER, 1); //将响应头信息和相应体一起传给write_data
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //设置为非0,响应头信息location, 设置支持302重定向 
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
		if (cookies)  curl_easy_setopt(curl, CURLOPT_COOKIE, cookies);

		//curl_easy_setopt(curl, CURLOPT_REFERER, url);//设置header中"Referer: " 部分的值。
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);			
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L); //进度条
		if (ip) curl_easy_setopt(curl, CURLOPT_INTERFACE, ip);//在外部网络接口中使用的名称，可以是一个接口名，IP或者主机名。
		if (timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
			if (res == CURLE_OPERATION_TIMEDOUT)
			{
				curl_easy_cleanup(curl);
				return -1;
			}
			curl_easy_cleanup(curl);
			return -2;
		}

		res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
		if (res != CURLE_OK || retcode != 200) {
			curl_easy_cleanup(curl);
			return -3;
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	//fclose(fptr);
	return buf.offset;
}