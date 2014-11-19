#ifndef TCPACCEPT_HPP
#define TCPACCEPT_HPP

#include <netinet/in.h>
#include "handler.hpp"

class Reactor;

struct BufBlock_t;

class TcpAccept : public HandlerBase {

public:
    TcpAccept(Reactor& reactor);
    virtual ~TcpAccept();

public:
    bool start(sockaddr_in &ip, uint32_t time_out);

public:
    bool handle_input();
    bool handle_output();
    void handle_error();
    bool push_buf(BufBlock_t* block);

private:
    Reactor& m_reactor;

    sockaddr_in m_ip;
    int m_fd;
    uint32_t m_timeout;
};

#endif
