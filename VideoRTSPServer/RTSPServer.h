#pragma once

#include "EdoyunThread.h"
#include "Socket.h"
#include <string>
#include <map>

using namespace std;

enum RTSPR {
	RTSPR_OPTIONS = 0,
	RTSPR_DESCRIBE = 1,
	RTSPR_SETUP = 2,
	RTSPR_PLAY = 3,
	RTSPR_TEARDOWN = 4
};

// RTSP������
class RTSPRequest {
public:
	RTSPRequest();
	RTSPRequest(const RTSPRequest& protocol);
	RTSPRequest& operator=(const RTSPRequest& protocol);
	~RTSPRequest();

private:
	RTSPR m_method;

};

// RTSP��Ӧ��
class RTSPReply {
public:
	RTSPReply();
	RTSPReply(const RTSPReply& protocol);
	RTSPReply& operator=(const RTSPReply& protocol);
	~RTSPReply();
	
private:
	RTSPR m_method;

};

class RTSPSession {
public:
	RTSPSession();
	RTSPSession(const RTSPSession& sess);
	RTSPSession& operator=(const RTSPSession& sess);
	~RTSPSession();
	
};

class RTSPServer : public ThreadFuncBase
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

protected:
	// ���߳�ִ�к���
	int threadWorker();

	// session�߳�ִ�к���
	int threadSession();

	// ��������
	RTSPRequest analyzeRequest(const string& data);
	
	// ��Ӧ����
	RTSPReply makeReply(const RTSPRequest& request);

private:
	static SocketIniter	s_initer;
	ESocket				m_socket;
	int					m_status;	// 0-δ��ʼ�� 1-��ʼ����� 2-�������� 3-�ر�
	EdoyunThread		m_threadMain;
	EdoyunThreadPool	m_pool;		// session�̳߳�
	map<string, RTSPSession> m_mapSession;
};

