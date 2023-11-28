#pragma once

#include <WinSock2.h>
#include <string>
#include <memory>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class EBuffer : public std::string {
public:
	EBuffer(const char* str) {
		resize(strlen(str));
		memcpy((void*)c_str(), str, size());
	}

	EBuffer(size_t size = 0) : string() {
		if (size > 0) {
			resize(size);
			memset(*this, 0, this->size());
		}
	}

	EBuffer(void* buffer, size_t size) : std::string() {
		resize(size);
		memcpy((void*)c_str(), buffer, size);
	}

	~EBuffer() {
		std::string::~basic_string();
	}

	operator char* () const { return (char*)c_str(); }
	operator const char* () const { return c_str(); }
	operator BYTE* () const { return (BYTE*)c_str(); }
	operator void* () const { return (void*)c_str(); }
	void update(void* buffer, size_t size) {
		resize(size);
		memcpy((void*)c_str(), buffer, size);
	}

	// 清空数据，但不改变字符串长度
	void zero() {
		if (size() > 0) {
			memset((void*)c_str(), 0, size());
		}
	}

	EBuffer& operator<< (const EBuffer& str) {
		if (this != str) {
			*this += str;
		}
		else {
			EBuffer tmp = str;
			*this += tmp;
		}
		return *this;
	}

	EBuffer& operator<< (const string& str) {
		*this += str;
		return *this;
	}

	EBuffer& operator<< (const char* str) {
		*this += str;
		return *this;
	}

	EBuffer& operator<< (int data) {
		char s[16] = "";
		snprintf(s, sizeof(s), "%d", data);
		*this += s;
		return *this;
	}

	const EBuffer& operator>>(int& data) const {
		data = atoi(c_str());
		return *this;
	}

	const EBuffer& operator>>(short& data) const {
		data = (short)atoi(c_str());
		return *this;
	}

};

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

class EAddress {
public:
	EAddress() {
		m_port = -1;
		memset(&m_addr, 0, sizeof(m_addr));
		m_addr.sin_family = AF_INET;
	}

	EAddress(const EAddress& addr) {
		m_ip = addr.m_ip;
		m_port = addr.m_port;
		memcpy(&m_addr, &addr.m_addr, sizeof(sockaddr_in));
	}

	EAddress& operator=(const EAddress& addr) {
		if (this != &addr) {
			m_ip = addr.m_ip;
			m_port = addr.m_port;
			memcpy(&m_addr, &addr.m_addr, sizeof(sockaddr_in));	
		}
		return *this;
	}
	~EAddress() {}

	void update(const string& ip, short port) {
		m_ip = ip;
		m_port = port;
		m_addr.sin_port = htons(port);
		m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	}

	operator const sockaddr* () const {
		return (sockaddr*)&m_addr;
	}

	operator sockaddr* () const {
		return (sockaddr*)&m_addr;
	}

	operator const sockaddr_in*() const {
		return &m_addr;
	}

	int size() const { return sizeof(sockaddr_in); }

private:
	string m_ip;		// IP
	short m_port;		// 端口号
	sockaddr_in m_addr;
};

class ESocket {
public:
	ESocket(bool bIsTCP = true) 
		: m_socket(new Socket(bIsTCP))
		, m_bIsTCP(bIsTCP)
	{}

	ESocket(const ESocket& sock) 
		: m_socket(sock.m_socket)
		, m_bIsTCP(sock.m_bIsTCP)
	{}

	ESocket(SOCKET sock, bool isTcp)
		: m_socket(new Socket(sock))
		, m_bIsTCP(isTcp)
	{}

	ESocket& operator=(const ESocket& sock) {
		if (this != &sock) {
			m_socket = sock.m_socket;
		}
		return *this;
	}

	~ESocket() {
		m_socket.reset();
	}

	operator SOCKET() {
		return *m_socket;
	}

	int Bind(const EAddress& addr) {
		if (m_socket == nullptr) {
			m_socket.reset(new Socket(m_bIsTCP));
		}
		return ::bind(*m_socket, addr, addr.size());
	}

	int Listen(int backlog = 5) {
		return listen(*m_socket, backlog);
	}

	ESocket Accept(EAddress& addr) {
		int len = addr.size();
		SOCKET s = accept(*m_socket, addr, &len);
		return ESocket(s, m_bIsTCP);
	}

	int Connect(const EAddress& addr) {
		return connect(*m_socket, addr, addr.size());
	}

	// 从socket里接受数据 输出到buffer里
	int Recv(EBuffer& buffer) {
		return recv(*m_socket, buffer, buffer.size(), 0);
	}

	// 返回值： 大于0-发送数据的程度 等于0-对方关闭了ws连接 小于0-发送出错
	int Send(const EBuffer& buffer) {
		// 待优化 buffer较长的情况
		int idx = 0;
		char* pData = buffer;
		while (idx < (int)buffer.size()) {
			int ret = send(*m_socket, pData + idx, buffer.size() - idx, 0);
			if (ret < 0) {
				// 发送失败
				return ret;
			}
			if (0 == ret) {
				// 对方关闭了ws连接
				break;
			}

			// 加上已发送的数据长度
			idx += ret;
		}
		return send(*m_socket, buffer, buffer.size(), 0);
	}

	void Close() {
		m_socket.reset();
	}

private:
	shared_ptr<Socket>	m_socket;
	bool				m_bIsTCP;	// 是否为TCP连接
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