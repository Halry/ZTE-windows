#define VER_Major    1
#define VER_Minor    1
#define VER_Release  14          
#define VER_Build    18         
#define VER_Version    MAKELONG(MAKEWORD(VER_Major, VER_Minor), VER_Release)

#define _Stringizing(v)    #v
#define _VerJoin(a, b, c, d)  _Stringizing(a.b.c.d)

#define STR_BuildDate    TEXT(__DATE__)
#define STR_BuildTime    TEXT(__TIME__)
#define STR_BuilDateTime  TEXT(__DATE__) TEXT(" ") TEXT(__TIME__)
#define STR_Version    TEXT(_VerJoin(VER_Major, VER_Minor, VER_Release, VER_Build))


#define MAX_STRING 2048
#define CONFIGNAME TEXT("zte.conf")
//#define CONFIGNAME TEXT("ZWZZte.conf")

#ifdef _DEBUG
#define STR_AppName    TEXT("莞香客户端(内测版)")
#else
#define STR_AppName    TEXT("莞香客户端")
#endif

#define STR_Author    TEXT("YZX")
#define STR_Corporation    TEXT("莞香广科")
#define STR_Modify    TEXT("见心秋月白")

//#define STR_AppName    TEXT("ZWZ's ZTE认证客户端")
//#define STR_Author    TEXT("ZWZ")
//#define STR_Corporation    TEXT("ZWZ's Studio")

#define STR_WEB      TEXT("bbs.gxgk.cc/")
#define STR_WEB_URL    TEXT("http://") STR_WEB
#define STR_AUTHOR_URL	STR_WEB_URL TEXT("space-uid-207.html")
#define STR_Modify_URL	TEXT("http://www.lastorder.cn")
//#define STR_EmailUrl    TEXT("mailto:") STR_Email TEXT("?Subject=ZWZZte")

#ifdef _CHS
#define STR_Description    STR_AppName
#define STR_Copyright    TEXT("版权所有 (C) 2009\0")
#else // _CHS
#define STR_Description    TEXT("GDST's ZTE")
#define STR_Copyright    TEXT("CopyLeft (C) 2009-2015 \0")
#define Copyright     TEXT(STR_Copyright STR_Description)
#endif // _CHS














