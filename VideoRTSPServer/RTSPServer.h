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

// RTSP请求类
class RTSPRequest {
public:
	RTSPRequest();
	RTSPRequest(const RTSPRequest& protocol);
	RTSPRequest& operator=(const RTSPRequest& protocol);
	~RTSPRequest();

private:
	RTSPR m_method;

};

// RTSP响应类
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

	// 初始化
	int init(const string& strIP = "127.0.0.1", short port = 554);

	// 运行
	int invoke();
	
	// 停止
	void stop();

protected:
	// 主线程执行函数
	int threadWorker();

	// session线程执行函数
	int threadSession();

	// 解析请求
	RTSPRequest analyzeRequest(const string& data);
	
	// 响应请求
	RTSPReply makeReply(const RTSPRequest& request);

private:
	static SocketIniter	s_initer;
	ESocket				m_socket;
	int					m_status;	// 0-未初始化 1-初始化完成 2-正在运行 3-关闭
	EdoyunThread		m_threadMain;
	EdoyunThreadPool	m_pool;		// session线程池
	map<string, RTSPSession> m_mapSession;
};

