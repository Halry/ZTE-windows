
/**************************************************************************************
			The Luzj's Zte Project
			//////////////////////
			Copyleft ? 2009 Luzj
		Author:Luzj		QQ:86829232
		http://blog.csdn.net/luzjqq
		Email: luzjcn@gmail.com
	///////////////////////////////////
����Luzj's Zte��֤�˵�������

1������������漰��������֤�Ĺ��ܵ�ʵ�־���ͨ���ںз�����������δͨ���κβ�����������á�

2������������о�ѧϰ֮�ã�����ʹ�ñ���������˹�˾��ҵ���档

3����������������κ���ҵ�ͷǷ���;�����������Ը���

4��������ڷ���ǰ��ͨ��һ����Ӧ�ò��ԣ�������֤�κ�����¶Ի����޺���
����δ֪��ʹ�û����򲻵���ʹ�öԼ������ɵ��𺦣�������ʹ����ȫ���е���

5.�������Ȩû�У���ӡ����������Э���Ľ�����Ʒ��

6.���������Դ����������Ҫ�޸ı����Դ���Խ��ж��η�����Ҳ���������Դ���롣

�����κβ�����������������ľ��ף����뱾���޹أ��粻ͬ��������벻Ҫʹ�ø������лл������
**************************************************************************************/

#include "stdafx.h"
#include "PicStatic.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPicStatic

CPicStatic::CPicStatic()
{
	pPic=NULL;
	nFileSize=0;
	x=y=0;
	GetPic();
}

CPicStatic::~CPicStatic()
{
}


BEGIN_MESSAGE_MAP(CPicStatic, CStatic)
	//{{AFX_MSG_MAP(CPicStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPicStatic message handlers

void CPicStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	if (pPic!=NULL)
	{
		ShowJpeg(pPic,nFileSize,x,y,&dc);
	}
	// Do not call CStatic::OnPaint() for painting messages
}

void CPicStatic::GetPic()
{
	HRSRC picSrc;
	HGLOBAL picGbl;
	if ((picSrc=FindResource(GetModuleHandle(NULL),MAKEINTRESOURCE(133),"jpg"))!=NULL)
	{
		if ((picGbl=LoadResource(GetModuleHandle(NULL),picSrc))!=NULL)
		{
			pPic = LockResource(picGbl);
			nFileSize = SizeofResource(GetModuleHandle(NULL), picSrc);
		}
		else
		{
			MessageBox("�޷����س��򶥲�ͼƬ");
		}
	}
	else
	{
		MessageBox("û�ҵ����򶥲�ͼƬ");
	}
}

HRESULT CPicStatic::ShowJpeg(LPVOID picData,DWORD dwFileSize,int x, int y,CDC *pDC)
{
	IStream   *pStm;
	IPicture   *pPic;
	BOOL   bResult;     
   
	HGLOBAL   hGlobal   =   GlobalAlloc(GMEM_MOVEABLE,   dwFileSize);     
	LPVOID   pvData   =   NULL;
	if (hGlobal  ==   NULL)
		return   false;
	pvData   =   GlobalLock(hGlobal);
	if ( pvData ==   NULL)  
		return   false;

	memcpy(pvData,picData,dwFileSize);
	GlobalUnlock(hGlobal);
	CreateStreamOnHGlobal(hGlobal,   TRUE,   &pStm);
	bResult=OleLoadPicture(pStm,dwFileSize,TRUE,IID_IPicture,(LPVOID*)&pPic);

	if(FAILED(bResult))
	{
//		CloseHandle(hFile);
		return   false;
	}
	OLE_XSIZE_HIMETRIC   hmWidth;
	OLE_YSIZE_HIMETRIC   hmHeight;
	pPic->get_Width(&hmWidth);     
	pPic->get_Height(&hmHeight);     
	//��ͼ���������Ļ�ϣ��е���BitBlt��     
	bResult=pPic->Render(pDC->m_hDC,0,0,x,y,0,hmHeight,hmWidth,-hmHeight,NULL);     

//	CloseHandle(hFile);
	pStm->Release();
	pPic->Release();
	if   (SUCCEEDED(bResult))     
	{
		return   S_OK;     
	}     
	else     
	{     
		return   E_FAIL;     
	}   
}
