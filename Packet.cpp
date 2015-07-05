
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


// Packet.cpp: implementation of the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Luzj_ZTE.h"
#include "Packet.h"
#include "md5.h"
#include "rc4.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacket::CPacket()
{
	static unsigned char default_broadcast_mac[6] = {0x01,0x80,0xc2,0x00,0x00,0x03};
	//static unsigned char default_broadcast_mac[6] = {0x00,0x19,0xc6,0x31,0xfc,0x77};	
	set_broadcast_mac(default_broadcast_mac);
}

CPacket::CPacket(const unsigned char *mac)
{
	set_broadcast_mac(mac);
}

CPacket::~CPacket()
{

}

int CPacket::set_broadcast_mac(const unsigned char *mac)
{
	memcpy(broadcast_mac, mac, 6);
	return 0;
}

//======================packet_start======================================
int CPacket::send_packet_start(pcap_t * adapterHandle,u_char* MacAdd)
{
	u_char packet_start[]={
					0x01,0x80,0xc2,0x00,0x00,0x03,			//�Է�MAC
					0x00,0x00,0x00,0x00,0x00,0x00,			//�Լ�MAC
					0x88,0x8e,0x01,0x01,0x00,0x00	};

	memcpy(packet_start,broadcast_mac,6);
	memcpy(packet_start+6,MacAdd,6);

	if(pcap_sendpacket(adapterHandle, packet_start,18)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_start======================================


//======================packet_logoff======================================
int CPacket::send_packet_logoff(pcap_t * adapterHandle,u_char* MacAdd)
{
	u_char packet_logoff[100]={
		0x01,0x80,0xc2,0x00,0x00,0x03,			//�Է�MAC
		0x00,0x00,0x00,0x00,0x00,0x00,						//�Լ�MAC
		0x88,0x8e,0x01,0x02,0x00,0x00	};

	memcpy(packet_logoff,broadcast_mac,6);
	memcpy(packet_logoff+6,MacAdd,6);

	if(pcap_sendpacket(adapterHandle, packet_logoff,18)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_logoff======================================


//======================packet_response_Identity======================================
int CPacket::send_packet_response_Identity(pcap_t * adapterHandle,const u_char * captured,u_char* MacAdd
											,u_char* m_username,int m_usernameLen)
{
	u_char packet_response_Identity[100]={	
					/*0*/0x01,0x80,0xc2,0x00,0x00,0x03,			//�Է�MAC
					/*6*/0x00,0x00,0x00,0x00,0x00,0x00,			//�Լ�MAC
					/*12*/0x88,0x8e,								//����Э������
					/*14*/0x01,									//Version: 1
					/*15*/0x00,									//Type: EAP Packet (0)
					/*16*/0x00,0x00,								//���ȣ�ʮ������11����10���Ƶ�17
					/*18*/0x02,									//Code: Response (2)
					/*19*/0x00,									//Id: �ɷ����İ�����
					/*20*/0x00,0x00,								//Length: 17
					/*22*/0x01,									//Type: Identity [RFC3748] (1)
					/*23*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00  //����Լ����˺ţ���ascii��
				};
	packet_response_Identity[17] = (u_char)m_usernameLen + 5;
	packet_response_Identity[21] = (u_char)m_usernameLen + 5;

	memcpy(packet_response_Identity,broadcast_mac,6);
	memcpy(packet_response_Identity+6,MacAdd,6);

	//����request����id
	packet_response_Identity[19]=captured[19];

	//���õ�¼���˺�
	for (int i=0;i<m_usernameLen;i++)
	{
		packet_response_Identity[23+i]=m_username[i];
	}

	if(pcap_sendpacket(adapterHandle, packet_response_Identity,23+m_usernameLen)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_response_Identity======================================


//======================packet_response_MD5======================================
int CPacket::send_packet_response_MD5(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd
									   ,u_char*	m_username,int m_usernameLen,u_char* m_password,int	m_passwordLen)
{	
	u_char packet_response_MD5[100]={	
					/*0*/0x01,0x80,0xc2,0x00,0x00,0x03,			//�Է�MAC
					/*6*/0x00,0x00,0x00,0x00,0x00,0x00,			//�Լ�MAC
					/*12*/0x88,0x8e,								//����
					/*14*/0x01,									//Version: 1
					/*15*/0x00,									//Type: EAP Packet (0)
					/*16*/0x00,0x22,								//���ȣ�22����10���Ƶ�34
					/*18*/0x02,									//Code: Response (2)
					/*19*/0x36,									//Id: 54
					/*20*/0x00,0x22,								//Length: 34
					/*22*/0x04,									//Type: MD5-Challenge [RFC3748] (4)
					/*23*/0x10,									//Value-Size: 16
					/*24*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�MD5��ǰ8�ֽ�
					/*32*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�MD5�ĺ�8�ֽ�
					/*40*/0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
						  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00//����Լ����˺ţ���ascii�룬��У�˺ų���Ϊ11
				};
	packet_response_MD5[17] = (u_char)m_usernameLen + 22;
	packet_response_MD5[21] = (u_char)m_usernameLen + 22;

	memcpy(packet_response_MD5,broadcast_mac,6);
	memcpy(packet_response_MD5+6,MacAdd,6);

	//MD5-Challenge�İ�ID
	packet_response_MD5[19]=captured[19];


/////////////////////////////////////////////////////////////////////
//����MD5
	u_char MD5Res[16] = {0};
	MD5_CTX context;
	BYTE	msgbuf[128]={0};
	int i=0,j=0;
	
	//������MD5-Challenge�İ�ID
	msgbuf[i++]=captured[19];
	
	//�����ǵ�½����
	while (j<m_passwordLen)
	{
		msgbuf[i++]=m_password[j++];
	}	
	
	//�����ǳ���zte142052
	u_char temp[]="zte142052";	
	memcpy(msgbuf+i,temp,9);
	i+=9;
	
	//������������MD5-Challenge
	memcpy(msgbuf+i,captured+24,16);
	i+=16;
	
	//ȡ��MD5��ϢժҪ�������MD5Res��
	MD5Init(&context);
	MD5Update(&context, msgbuf, i);
	MD5Final(MD5Res, &context);
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


	//MD5-Challenge:
	memcpy(packet_response_MD5+24,MD5Res,16);

	//�����ǵ�½���˺�
	for (i=0;i<m_usernameLen;i++)
	{
		packet_response_MD5[40+i]=m_username[i];
	}


	if(pcap_sendpacket(adapterHandle, packet_response_MD5,40+m_usernameLen)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_response_MD5======================================


//======================packet_key1======================================
int CPacket::send_packet_key1(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd)
{
	u_char packet_key1[100]={	
					0x01,0x80,0xc2,0x00,0x00,0x03,				//�Է�MAC
					0x00,0x00,0x00,0x00,0x00,0x00,				//�Լ�MAC
					0x88,0x8e,									//����
					0x01,										//Version: 1
					0x03,										//Type: EAP Packet (3)
					0x00,0x3c,									//���ȣ�3c����10���Ƶ�60
					0x01,										//Descriptor Type: RC4 Descriptor (1)
					0x00,0x10,									//key length
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Replay Counter: 8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key IV ǰ8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key IV ��8�ֽ�
					0x00,										//index	
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key Signature ǰ8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key Signature ��8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key ǰ8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key ��8�ֽ�
			};


	//MAC
	memcpy(packet_key1,broadcast_mac,6);
	memcpy(packet_key1+6,MacAdd,6);

	//Replay Counter  +  Key IV ԭ�����ƣ�24�ֽ�
	memcpy(packet_key1+21,captured+21,24);

	//�˰���indexֵ��ֱ�Ӵӽ��հ����ƹ���
	packet_key1[45]=captured[45];
	
	//////////////////////////////////////////////////////////////////////////
	//ʹ��rc4�㷨����Key�����ڣ�Key IV + Key IV����ĸ��ֽڣ�==20�ֽ�
	u_char enckey[]={0x02,0x0E,0x05,0x04,0x66,0x40,0x19,0x75
					,0x06,0x06,0x00,0x16,0xD3,0xF3,0xAC,0x02
	};
	// 	u_char enckey[]={0x02,0x0E,0x05,0x04,0xD5,0x40,0x19,0x75
	// 		,0x06,0x06,0x00,0x13,0x77,0x4C,0x4E,0xAB
	// 	};
	u_char wholekey[20];
	memcpy(wholekey,captured+29,16);
	memcpy(wholekey+16,captured+41,4);
	int keylen=16;
	struct rc4_state s;
	rc4_setup( &s, wholekey, 20 );
    rc4_crypt( &s, enckey, keylen );
	memcpy(&packet_key1[62],enckey,16);

	//////////////////////////////////////////////////////////////////////////
	//ʹ��hmac_md5�㷨����Key Signature�������ڰ���У��
	u_char deckey[64]={0};
	u_char encDat[64];
	memcpy(encDat,packet_key1+14,64);
	enckey[0]=packet_key1[45];
	hmac_md5(encDat,64,enckey,1,deckey);
	memcpy(packet_key1+46,deckey,16);

	if(pcap_sendpacket(adapterHandle, packet_key1,78)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_key1======================================


//======================packet_key2======================================
int CPacket::send_packet_key2(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd)
{
	u_char packet_key2[100]={
					0x01,0x80,0xc2,0x00,0x00,0x03,				//�Է�MAC
					0x00,0x00,0x00,0x00,0x00,0x00,				//�Լ�MAC
					0x88,0x8e,									//����
					0x01,										//Version: 1
					0x03,										//Type: EAP Packet (3)
					0x00,0x30,									//���ȣ�3c����10���Ƶ�48
					0x01,										//Descriptor Type: RC4 Descriptor (1)
					0x00,0x04,									//key length
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//Replay Counter: 8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key IV ǰ8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key IV ��8�ֽ�
					0x00,										//index	
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key Signature ǰ8�ֽ�
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	//16�ֽڵ�Key Signature ��8�ֽ�
					0x00,0x00,0x00,0x00							//4�ֽڵ�Key
			};
	
	//MAC
	memcpy(packet_key2,broadcast_mac,6);
	memcpy(packet_key2+6,MacAdd,6);
	
	//Replay Counter  +  Key IV ԭ�����ƣ�24�ֽ�
	memcpy(packet_key2+21,captured+21,24);
	
	//�˰���indexֵ��ֱ�Ӵӽ��հ����ƹ���
	packet_key2[45]=captured[45];

	//////////////////////////////////////////////////////////////////////////
	//ʹ��rc4�㷨����Key�����ڣ�Key IV + Key IV����ĸ��ֽڣ�==20�ֽ�
	u_char enckey[]={0x02,0x02,0x14,0x00};
	u_char wholekey[20];
	memcpy(wholekey,captured+29,16);
	memcpy(wholekey+16,captured+41,4);
	int keylen=4;
	u_char deckey[64]={0};
	struct rc4_state s;
	rc4_setup( &s, wholekey, 20 );
    rc4_crypt( &s, enckey, keylen );
	memcpy(packet_key2+62,enckey,4);

	//////////////////////////////////////////////////////////////////////////
	//ʹ��hmac_md5�㷨����Key Signature�������ڰ���У��	
	u_char encDat[64];
	memcpy(encDat,packet_key2+14,52);
	enckey[0]=packet_key2[45];
	hmac_md5(encDat,52,enckey,1,deckey);
	memcpy(packet_key2+46,deckey,16);


	if(pcap_sendpacket(adapterHandle, packet_key2,66)!=0)
    {
        return 0;
    }
	return 1;
}
//======================packet_key2======================================

