#pragma once

#include <WinSock2.h>
#include <share.h>

using namespace std;

class Socket
{
public:
	Socket(bool bIsTCP = true) 
		: m_sock(INVALID_SOCKET)
	{
		if (bIsTCP) {
			m_sock = socket(PF_INET, SOCK_STREAM, 0);
		}
		else {
			m_sock = socket(PF_INET, SOCK_DGRAM, 0);
		}
	}

	Socket(SOCKET s){
		m_sock = s;
	}
		
	~Socket() {
		close();
	}
		

	void close() {
		if (m_sock != INVALID_SOCKET) {
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
		}
	}

	operator SOCKET() {
		return m_sock;
	}

private:
	SOCKET	m_sock;
};

class ESocket {
public:
	ESocket(bool bIsTCP = true) 
		: m_socket(new Socket(bIsTCP))
	{}

	ESocket(const ESocket& sock) 
		: m_socket(sock.m_socket){}

	ESocket& operator=(const ESocket& sock) {
		if (this != &sock) {
			m_socket = sock.m_socket;
			return *this;
		}
	}

	~ESocket() {
		m_socket.reset();
	}

	operator SOCKET() {
		return *m_socket;
	}

private:
	shared_ptr<Socket>	m_socket;
};

class SocketIniter 
{
public:
	SocketIniter() {
		WSADATA wsa;
		WSAStartup(MAKEWORD(2, 2), &wsa);
	}
	~SocketIniter() {
		WSACleanup();
	}

};