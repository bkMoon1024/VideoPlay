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

// RTSP请求类
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
	EBuffer	m_clientPort[2];		// RTP端口和RTCP端口，UDP套接字


};

// RTSP响应类
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
	short	m_client_port[2];	// RTP端口和RTCP端口，UDP套接字
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
	
	// 解析并返回请求
	int pickRequestAndReply();

private:
	EBuffer pickOneLine(EBuffer& buffer);

	// 获取请求
	EBuffer pick();

	// 解析请求
	RTSPRequest analyzeRequest(const EBuffer& buffer);

	// 回应请求
	RTSPReply reply(const RTSPRequest& request);


private:
	string	m_id;		// session ID:
	ESocket m_client;	// session对应的客户端
};



class RTSPServer : public ThreadFuncBase
{
public :
	RTSPServer() : m_socket(true), m_status(0), m_pool(10) {
		m_threadMain.UpdateWorker(::ThreadWorker(this, (FUNCTYPE)&RTSPServer::threadWorker));
	};
	~RTSPServer();

	// 初始化
	int init(const string& strIP = "0.0.0.0", short port = 554);

	// 运行
	int invoke();
	
	// 停止
	void stop();

protected:
	// 主线程执行函数 返回0-继续 负数-终止 其他-警告
	int threadWorker();

	// session线程执行函数
	int threadSession();

private:
	static SocketIniter	s_initer;
	ESocket				m_socket;
	EAddress			m_addr;		
	int					m_status;	// 0-未初始化 1-初始化完成 2-正在运行 3-关闭
	EdoyunThread		m_threadMain;
	EdoyunThreadPool	m_pool;		// session线程池
	CEdoyunQueue<RTSPSession>	m_sessionList;	// 连接的客户端
};

