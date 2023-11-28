#include "RTSPServer.h"
#include <rpc.h>

#pragma comment(lib, "rpcrt4.lib")

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
        RTSPSession session(client);
        m_sessionList.PushBack(session);
        m_pool.DispatchWorker(ThreadWorker(this, (FUNCTYPE)&RTSPServer::threadSession));
    }
   
    return 0;
}

int RTSPServer::threadSession()
{
    // 接受数据请求
    RTSPSession sess; // 假设可以拿到ESocket变量
    if (!m_sessionList.PopFront(sess)) {
        return -1;
    }
   
    return sess.pickRequestAndReply();
}

RTSPSession::RTSPSession()
{
    // 生成唯一的session ID
    UUID uuid;
    UuidCreate(&uuid);
    m_id.resize(10);
    snprintf((char*)m_id.c_str(), m_id.size(), "%08d", uuid.Data1);
}

RTSPSession::RTSPSession(const ESocket& client)
    : m_client(client)
{
    // 生成唯一的session ID
    UUID uuid;
    UuidCreate(&uuid);
    m_id.resize(10);
    snprintf((char*)m_id.c_str(), m_id.size(), "%08d", uuid.Data1);
}

RTSPSession::RTSPSession(const RTSPSession& sess)
{
    m_id = sess.m_id;
    m_client = sess.m_client;
}

RTSPSession& RTSPSession::operator=(const RTSPSession& sess)
{
    // TODO: 在此处插入 return 语句
    if (this != &sess) {
        m_id = sess.m_id;
        m_client = sess.m_client;
    }
    return *this;
}

int RTSPSession::pickRequestAndReply()
{
    // 获取请求
    EBuffer buffer = pick();

    // 解析请求
    RTSPRequest req = analyzeRequest(buffer);

    if (req.method() < 0) {
        // 检查该请求是否正常 默认req.method()为-1 
        // method应在0~4范围内
        TRACE("buffer [%s]\r\n", (char*)buffer);
        return -2;
    }
    
    // 回应请求
    RTSPReply rep = reply(req);

    int ret = m_client.Send(rep.toBuffer());
    if (ret < 0) {
        return ret;
    }

    return 0;
}

EBuffer RTSPSession::pickOneLine(EBuffer& buffer)
{
    EBuffer result, temp;
    if (buffer.size() <= 0) {
        return result;
    }

    int i = 0;
    for (; i < (int)buffer.size(); ++i) {
        result += buffer.at(i);
        if (buffer.at(i) == '\n') {
            i++;
            break;
        }
    }

    temp = i + (char*)buffer;
    buffer = temp;

    return result;
}

EBuffer RTSPSession::pick()
{
    EBuffer buffer;
    EBuffer b(1);
    int ret = 1;
        
    while (ret > 0) {;
        b.zero();
        ret = m_client.Recv(b);

        if (ret > 0) {
            buffer += b;

            if (buffer.size() >= 4) {
                // 判断是否已经接受到完整的数据
                // RTSP协议以 "\r\n\r\n结尾"
                UINT val = *(UINT*)((char*)buffer + buffer.size() - 4);
                if (val == *(UINT*)"\r\n\r\n") {
                    break;
                }
            }
        }
    }
    
    return buffer;
}

RTSPRequest RTSPSession::analyzeRequest(const EBuffer& buffer)
{
    EBuffer data = buffer;
    RTSPRequest req;
    
    // 获取 METHOD URL VERSION
    EBuffer line = pickOneLine(data);
    EBuffer method(32), url(1024), version(16);
    if (sscanf(line, "%s %s %s\r\n", (char*)method, (char*)url, (char*)version) < 3) {
        TRACE("Error at :[%s]\r\n", (char*)line);
        return req;
    }

    // 获取CSeq
    EBuffer seq(64);
    line = pickOneLine(data);
    if (sscanf(line, "CSeq: %s\r\n", (char*)seq) < 1) {
        TRACE("Error at :[%s]\r\n", (char*)line);
        return req;
    }

    req.setMethod(method);
    req.setUrl(url);
    req.setSequence(seq);
    if ((strcmp(method, "OPTIONS") == 0) || (strcmp(method, "DESCRIBE") == 0)) {
        return req;
    }
    else if (strcmp(method, "SETUP") == 0) {
        line = pickOneLine(data);
        short port[2] = { 0, 0 };
        if(sscanf(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", port, port + 1) == 2){
            req.setClientPort(port);
            return req;
        }

    }
    else if ((strcmp(method, "PLAY") == 0) || (strcmp(method, "TEARDOWN") == 0)) {
        // 获取session
        line = pickOneLine(data);
        EBuffer sess(64);
        if (sscanf(line, "Session: %s\r\n", (char*)sess) == 1) {
            req.setSession(sess);
            return req;
        }
    }

    return req;
}

RTSPReply RTSPSession::reply(const RTSPRequest& request)
{
    RTSPReply rep;
    rep.setSequence(request.sequence());
    if (request.session().size() > 0) {
        rep.setSession(request.session());
    }
    switch (request.method()) {
    case RTSPR_OPTIONS:
        break;
    case RTSPR_DESCRIBE:{
        EBuffer sdp;
        sdp << "v=0\r\n";
        sdp << "o=-" << m_id << " 1 IN IP4 127.0.0.1\r\n";
        sdp << "t=0 0\r\n" << "a=control:*\r\n" << "m=video 0 RTP/AVP 96\r\n";
        sdp << "a=rtpmap:96 H264/90000\r\n" << "a=control:track0\r\n";
        rep.setSdp(sdp);
    }
        break;
    case RTSPR_SETUP:
        rep.setClientPort(request.clientPort(0), request.clientPort(1));
        rep.setServerPort("55000", "55001");
        break;
    case RTSPR_PLAY:
    case RTSPR_TEARDOWN:
        break;
    default:
        break;
    }

    return rep;
}

RTSPRequest::RTSPRequest()
{
    m_method = RTSPR_UNKNOWN;
}

RTSPRequest::RTSPRequest(const RTSPRequest& protocol)
{
    m_method = protocol.m_method;
    m_url = protocol.m_url;
    m_session = protocol.m_session;
    m_seq = protocol.m_seq;
    m_clientPort[0] = protocol.m_clientPort[0];
    m_clientPort[1] = protocol.m_clientPort[1];
}

RTSPRequest& RTSPRequest::operator=(const RTSPRequest& protocol)
{
    // TODO: 在此处插入 return 语句
    if (this != &protocol) {
        m_method = protocol.m_method;
        m_url = protocol.m_url;
        m_session = protocol.m_session;
        m_seq = protocol.m_seq;
        m_clientPort[0] = protocol.m_clientPort[0];
        m_clientPort[1] = protocol.m_clientPort[1];
    }
    return *this;
}

RTSPRequest::~RTSPRequest()
{
    m_method = RTSPR_UNKNOWN;
}

void RTSPRequest::setUrl(const EBuffer& url)
{
    m_url = url;
}

void RTSPRequest::setSession(const EBuffer& sess)
{
    m_session = sess;
}

void RTSPRequest::setMethod(const EBuffer& method)
{
    if (method == "OPTIONS") {
        m_method = RTSPR_OPTIONS;
    }
    else if (method == "DESCRIBE") {
        m_method = RTSPR_DESCRIBE;
    }
    else if (method == "SETUP") {
        m_method = RTSPR_SETUP;
    }
    else if (method == "PLAY") {
        m_method = RTSPR_PLAY;
    }
    else if (method == "TEARDOWN") {
        m_method = RTSPR_TEARDOWN;
    }
    else {
        m_method = RTSPR_UNKNOWN;
    }
}

void RTSPRequest::setSequence(const EBuffer& seq)
{
    m_seq = seq;
}

void RTSPRequest::setClientPort(short port[])
{
    m_clientPort[0] << port[0];
    m_clientPort[1] << port[1];
}

RTSPR RTSPRequest::method() const
{
    return m_method;
}

const EBuffer& RTSPRequest::url() const
{
    // TODO: 在此处插入 return 语句
    return m_url;
}

const EBuffer& RTSPRequest::session() const
{
    // TODO: 在此处插入 return 语句
    return m_session;
}

const EBuffer& RTSPRequest::sequence() const
{
    // TODO: 在此处插入 return 语句
    return m_seq;
}

const EBuffer& RTSPRequest::clientPort(int idx) const
{
    // TODO: 在此处插入 return 语句
    return idx ? m_clientPort[1] : m_clientPort[0];
}

RTSPReply::RTSPReply()
{
    m_method = RTSPR_UNKNOWN;
}

RTSPReply::RTSPReply(const RTSPReply& protocol)
{
    m_method = protocol.m_method;
    m_client_port[0] = protocol.m_client_port[0];
    m_client_port[1] = protocol.m_client_port[1];
    m_server_port[0] = protocol.m_server_port[0];
    m_server_port[1] = protocol.m_server_port[1];
    m_seq = protocol.m_seq;
    m_sdp = protocol.m_sdp;
    m_options = protocol.m_options;
    m_session = protocol.m_session;
}

RTSPReply& RTSPReply::operator=(const RTSPReply& protocol)
{
    // TODO: 在此处插入 return 语句
    if (this != &protocol) {
        m_method = protocol.m_method;
        m_client_port[0] = protocol.m_client_port[0];
        m_client_port[1] = protocol.m_client_port[1];
        m_server_port[0] = protocol.m_server_port[0];
        m_server_port[1] = protocol.m_server_port[1];
        m_seq = protocol.m_seq;
        m_sdp = protocol.m_sdp;
        m_options = protocol.m_options;
        m_session = protocol.m_session;
    }
    return *this;
}

RTSPReply::~RTSPReply()
{
    m_method = RTSPR_UNKNOWN;
}

EBuffer RTSPReply::toBuffer()
{
    EBuffer result;
    result << "RTSP/1.0 200 OK \r\n";
    result << "CSeq: " << m_seq << "\r\n";
    switch (m_method) {
    case RTSPR_OPTIONS:
        result << "Public: OPTIONS, DESCRIBE, SETUP, PLAY, TEARDOWN\r\n\r\n";
        break;
    case RTSPR_DESCRIBE:
        result << "Content-Base: 127.0.0.1\r\n";
        result << "Content-length: " << m_sdp.size() << "\r\n";
        result << "Content-type: application/sdp\r\n\r\n";
        result << m_sdp;
        break;
    case RTSPR_SETUP:
        result << "Transport: RTP/AVP;unicast;client_port="
            << m_client_port[0] << "-" << m_client_port[1] << ";"
            << "server_port=" << m_server_port[0] << "-" << m_server_port[1] << "\r\n";
        result << "Session: " << m_session << "\r\n\r\n";
        break;
    case RTSPR_PLAY:
        result << "Range: npt=0.000-\r\n";
        result << "Session: " << m_session << "\r\n\r\n";
    case RTSPR_TEARDOWN:
        result << "Session: " << m_session << "\r\n\r\n";
        break;
    case RTSPR_UNKNOWN:
    default:
        break;
    }
    return result;
}

void RTSPReply::setOptions(const EBuffer& options)
{
    m_options = options;
}

void RTSPReply::setSequence(const EBuffer& seq)
{
    m_seq = seq;
}

void RTSPReply::setSdp(const EBuffer& sdp)
{
    m_sdp = sdp;
}

void RTSPReply::setClientPort(const EBuffer& port0, const EBuffer& port1)
{   
    port0 >> m_client_port[0];
    port1 >> m_client_port[1];
}

void RTSPReply::setServerPort(const EBuffer& port0, const EBuffer& port1)
{
    port0 >> m_server_port[0];
    port1 >> m_server_port[1];
}

void RTSPReply::setSession(const EBuffer& sess)
{
    m_session = sess;
}
