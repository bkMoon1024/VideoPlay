#include "RTSPServer.h"

RTSPServer::~RTSPServer()
{
    stop();
}

int RTSPServer::init(const string& strIP, short port)
{
    m_addr.update(strIP, port);
    m_socket.Bind(m_addr);
    m_socket.Listen();
    
    return 0;
}

int RTSPServer::invoke()
{
    m_threadMain.Start();
    return 0;
}

void RTSPServer::stop()
{
    m_socket.Close();
    m_threadMain.Stop();
    m_pool.Stop();
}


int RTSPServer::threadWorker()
{
    EAddress clientAddr;
    ESocket client = m_socket.Accept(clientAddr);
    if (client != INVALID_SOCKET) {
        m_clients.PushBack(client);
        m_pool.DispatchWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::threadSession));
    }
   
    return 0;
}

int RTSPServer::threadSession()
{
    // ������������
    ESocket client; // ��������õ�ESocket����
    EBuffer buffer(1024 * 16);
    int len = client.Recv(buffer);

    if (len <= 0) {
        // ����client
        return -1;
    }

    buffer.resize(len);

    // ��������
    RTSPRequest request = analyzeRequest(buffer);

    // Ӧ������
    RTSPReply reply = makeReply(request);

    client.Send(reply.toBuffer());


    

   
    return 0;
}
