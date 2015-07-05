// Packet.h: interface for the CPacket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_)
#define AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000






class CPacket  
{
private:
	unsigned char broadcast_mac[6];
public:
	CPacket();
	CPacket(const unsigned char *mac);
	virtual ~CPacket();
	int set_broadcast_mac(const unsigned char *mac);

	//����Ϊ���ݰ���ʼ�������͵ĺ���
	int send_packet_start(pcap_t * adapterHandle,u_char* MacAdd);						//��Ϊ��ʼ�İ�

	int send_packet_logoff(pcap_t * adapterHandle,u_char* MacAdd);						//��Ϊ�˳��İ�

	int send_packet_response_Identity(pcap_t * adapterHandle,const u_char * captured,u_char* MacAdd
									,u_char* m_username,int m_usernameLen);				//��Ϊ��Ӧϵͳ��request��

	int send_packet_response_MD5(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd
		,u_char* m_username,int m_usernameLen,u_char* m_password,int m_passwordLen);	//��Ϊ��Ӧϵͳ��MD5-Challenge��

	int send_packet_key1(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd);		//����ά�ְ�1

	int send_packet_key2(pcap_t * adapterHandle,const u_char* captured,u_char* MacAdd);		//����ά�ְ�2

};

#endif // !defined(AFX_PACKET_H__AE293802_9E7F_4666_8323_49ECE7F9A8D0__INCLUDED_)
