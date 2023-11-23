#pragma once

#include "Socket.h"
#include <string>

using namespace std;

class RTSPServer
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


private:
	ESocket	m_socket;
	int		m_status;	// 0-未初始化 1-初始化完成 2-正在运行 3-关闭


};

