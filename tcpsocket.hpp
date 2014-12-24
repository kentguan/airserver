#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include <netinet/in.h>
#include "handler.hpp"
#include "singleton_buff.hpp"
#include "queue.hpp"

class Reactor;

class TcpSocket : public HandlerBase
{
public:
    TcpSocket(Reactor& reactor);
    virtual ~TcpSocket();

public:
    bool start(int fd, sockaddr_in &local_ip, sockaddr_in &ip, uint32_t conn_timeout);


public:
    virtual bool handle_input(); 
    virtual bool handle_output(); 
    virtual void handle_error(); 
    virtual bool push_buf(BufBlock_t* block);
    virtual void check_timeout();

private:
    Reactor& m_reactor;

    sockaddr_in m_ip;
    sockaddr_in m_local_ip;

    uint32_t m_conn_timeout;
    uint32_t m_last_time;

    Skinfo_t m_sk;

    int m_fd;
    int m_id;

    uint32_t m_sendpos;

    uint32_t m_recvpos;
    uint8_t* m_recvbuf;
    uint32_t m_recvbuf_size;

    Queue<BufBlock_t*> m_sendbufs;  //接受队列
};

#endif
