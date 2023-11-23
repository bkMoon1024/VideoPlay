#pragma once

#include "Socket.h"
#include <string>

using namespace std;

class RTSPServer
{
public :
	RTSPServer() : m_socket(true), m_status(0) {};
	~RTSPServer();

	// ��ʼ��
	int init(const string& strIP = "127.0.0.1", short port = 554);

	// ����
	int invoke();
	
	// ֹͣ
	void stop();


private:
	ESocket	m_socket;
	int		m_status;	// 0-δ��ʼ�� 1-��ʼ����� 2-�������� 3-�ر�


};

