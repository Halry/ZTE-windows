#include "stdafx.h"
#include "AutoUpdate.h"
#include "WebAuth.h"
#include "Config.h"

#define MAX_BUFFER_SIZE 10240

int UTF8ToGBK(unsigned char * lpUTF8Str,unsigned char * lpGBKStr,int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if(!lpUTF8Str) //���UTF8�ַ���ΪNULL������˳�
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,NULL,NULL); //��ȡת����Unicode���������Ҫ���ַ��ռ䳤��
	lpUnicodeStr = new WCHAR[nRetLen + 1]; //ΪUnicode�ַ����ռ�
	nRetLen = ::MultiByteToWideChar(CP_UTF8,0,(char *)lpUTF8Str,-1,lpUnicodeStr,nRetLen); //ת����Unicode����
	if(!nRetLen) //ת��ʧ��������˳�
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,NULL,NULL,NULL,NULL); //��ȡת����GBK���������Ҫ���ַ��ռ䳤��

	if(!lpGBKStr) //���������Ϊ���򷵻�ת������Ҫ�Ŀռ��С
	{
		if(lpUnicodeStr)
		delete []lpUnicodeStr;
		return nRetLen;
	}

	if(nGBKStrLen < nRetLen) //���������������Ȳ������˳�
	{
		if(lpUnicodeStr)
		delete []lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP,0,lpUnicodeStr,-1,(char *)lpGBKStr,nRetLen,NULL,NULL); //ת����GBK����

	if(lpUnicodeStr)
		delete []lpUnicodeStr;

	return nRetLen;
}

char * WebAuth(const char * username, const char *password, const char *ip, const char *base_url, const int timeout)
{	
	char url[MAX_BUFFER_SIZE] = { 0 };
#ifdef WIN32
#define snprintf _snprintf
#endif
	snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s&userName1=%s&password1=%s", base_url, ip, username, password);

	//memset(http, 0, MAX_BUFFER_SIZE*sizeof(char));

	return WebAuthMessages(url,ip,timeout);
}

char * WebLogout(const char *ip, const char *base_url, const int timeout)
{
	static char msg[MAX_BUFFER_SIZE];
	char url[MAX_BUFFER_SIZE] = {0};
	
	CURL *curl;
	CURLcode res;
	long retcode = 0;
	
	curl = curl_easy_init();
	if(curl) {	
		
#ifdef WIN32
#define snprintf _snprintf
#endif
		snprintf(url, MAX_BUFFER_SIZE, "%s&eduuser=%s", base_url, ip);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		if(timeout > 0) curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//ע��֮����Զ�ת��
		
		res = curl_easy_perform(curl);		
		if(res == CURLE_OK) {			
			res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(res == CURLE_OK && retcode == 200) {				
				char *r;
				res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &r);				
				if(res == CURLE_OK && strstr(r, "/logoutsuccess.jsp")) {
					curl_easy_cleanup(curl);
					return "logoutsuccess";
				}
				else if (res == CURLE_OK && strstr(r, "/logoutfailed.jsp")){
					curl_easy_cleanup(curl);
					return "logoutfailed";
				}
				else if(res == CURLE_OK){
					strncpy(msg, "��ҳ��ַ���ض���", MAX_BUFFER_SIZE);
					strcat(msg, r);
				}
			}
		}
		if(res != CURLE_OK)		
			strncpy(msg, curl_easy_strerror(res), MAX_BUFFER_SIZE);
		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	return msg;
}
char *WebAuthMessages(const char url[MAX_BUFFER_SIZE], const char *ip, const int timeout)
{
	static char msg[MAX_BUFFER_SIZE];
	static char http[MAX_BUFFER_SIZE] = { 0 };
	int ret = DownLoadFileToBuffer(http, MAX_BUFFER_SIZE, url, ip, timeout, NULL);
	if (ret == -1)
		return "��ҳ��֤���ӳ�ʱ";
#define LOCATE_STR "var alertInfo = \""
	char *p = strstr(http, LOCATE_STR);
	if (p == NULL)
		return "HTTP_BAD_FORMAT1";
	p += strlen(LOCATE_STR);
	char *q = strstr(p, "\"");
	if (q == NULL)
		return "HTTP_BAD_FORMAT2";
	*q = '\0';

	if (strcmp(p, "null") == 0) return NULL;
	//utf-8 to ascii
	//wcstombs(msg, (WCHAR*)p, 20);
	UTF8ToGBK((unsigned char*)p, (unsigned char*)msg, MAX_BUFFER_SIZE);
	return msg;
}