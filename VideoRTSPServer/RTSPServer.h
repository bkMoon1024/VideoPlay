#pragma once

#include "Socket.h"
#include "CEdoyunQueue.h"
#include "EdoyunThread.h"
#include <string>
#include <map>

using namespace std;

enum RTSPR {
	RTSPR_UNKNOWN = -1,
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

	void setUrl(const EBuffer& url);
	void setSession(const EBuffer& sess);
	void setMethod(const EBuffer& method);
	void setSequence(const EBuffer& seq);
	void setClientPort(short port[]);

	RTSPR method() const;
	const EBuffer& url() const;
	const EBuffer& session() const;
	const EBuffer& sequence() const;
	const EBuffer& clientPort(int idx = 0) const;

private:
	RTSPR	m_method;				// METHOD
	EBuffer	m_url;		
	EBuffer	m_session;
	EBuffer	m_seq;					// CSeq
	EBuffer	m_clientPort[2];		// RTP�˿ں�RTCP�˿ڣ�UDP�׽���


};

// RTSP��Ӧ��
class RTSPReply {
public:
	RTSPReply();
	RTSPReply(const RTSPReply& protocol);
	RTSPReply& operator=(const RTSPReply& protocol);
	~RTSPReply();
	
	EBuffer toBuffer();
	
	void setOptions(const EBuffer& options);
	void setSequence(const EBuffer& seq);
	void setSdp(const EBuffer& sdp);
	void setClientPort(const EBuffer& port0, const EBuffer& port1);
	void setServerPort(const EBuffer& port0, const EBuffer& port1);
	void setSession(const EBuffer& sess);

private:
	RTSPR	m_method;
	short	m_client_port[2];	// RTP�˿ں�RTCP�˿ڣ�UDP�׽���
	short	m_server_port[2];
	EBuffer m_seq;
	EBuffer	m_sdp;
	EBuffer m_options;
	EBuffer m_session;
};

class RTSPSession {
public:
	RTSPSession();
	RTSPSession(const ESocket& client);
	RTSPSession(const RTSPSession& sess);
	RTSPSession& operator=(const RTSPSession& sess);
	~RTSPSession(){}
	
	// ��������������
	int pickRequestAndReply();

private:
	EBuffer pickOneLine(EBuffer& buffer);

	// ��ȡ����
	EBuffer pick();

	// ��������
	RTSPRequest analyzeRequest(const EBuffer& buffer);

	// ��Ӧ����
	RTSPReply reply(const RTSPRequest& request);


private:
	string	m_id;		// session ID:
	ESocket m_client;	// session��Ӧ�Ŀͻ���
};



class RTSPServer : public ThreadFuncBase
{
public :
	RTSPServer() : m_socket(true), m_status(0), m_pool(10) {
		m_threadMain.UpdateWorker(::ThreadWorker(this, (FUNCTYPE)&RTSPServer::threadWorker));
	};
	~RTSPServer();

	// ��ʼ��
	int init(const string& strIP = "0.0.0.0", short port = 554);

	// ����
	int invoke();
	
	// ֹͣ
	void stop();

protected:
	// ���߳�ִ�к��� ����0-���� ����-��ֹ ����-����
	int threadWorker();

	// session�߳�ִ�к���
	int threadSession();

private:
	static SocketIniter	s_initer;
	ESocket				m_socket;
	EAddress			m_addr;		
	int					m_status;	// 0-δ��ʼ�� 1-��ʼ����� 2-�������� 3-�ر�
	EdoyunThread		m_threadMain;
	EdoyunThreadPool	m_pool;		// session�̳߳�
	CEdoyunQueue<RTSPSession>	m_sessionList;	// ���ӵĿͻ���
};

