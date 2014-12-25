#include <unistd.h>                                                                                                             
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <log4cplus/logger.h>

#include "tcpaccept.hpp"
#include "reactor.hpp"
#include "tcpsocketpool.hpp"

TcpAccept::TcpAccept(Reactor &reactor)
 : m_reactor(reactor), m_fd(-1) {
}

TcpAccept::~TcpAccept() {
}

bool TcpAccept::start(sockaddr_in &ip, uint32_t time_out) {
    
    m_ip = ip;
    m_timeout = time_out;
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_fd) {
        return false;
    }

    int f;
    f = fcntl(m_fd, F_GETFL, 0);
    f |= O_NONBLOCK;
    fcntl(m_fd, F_SETFL, f);

    int reuseaddr = 1;
    if (-1 == setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr, 
                sizeof(reuseaddr))) {
        return false;
    }

    if (-1 == bind(m_fd, (sockaddr*)&m_ip, sizeof(sockaddr_in))) {
        return false;
    }

    if (-1 == listen(m_fd, 1024)) {
        return false;
    }

    if (false == m_reactor.set_handler(m_fd, this)) {
        return false;
    }

    if (false == m_reactor.handle_ctl(m_fd, EPOLL_CTL_ADD, EPOLLIN)) {
        return false;
    }

    return true;
}

bool TcpAccept::handle_input() {

    sockaddr_in client_ip;
    socklen_t ip_size;

    int fd;
    while (1) {
        ip_size = sizeof(sockaddr_in);

        fd = accept(m_fd, (sockaddr*)&client_ip, &ip_size);
        if (fd >= 0) {
            if (fd > m_reactor.handle_size()) {
                close(fd);
                continue;
            }

            TcpSocket *ts = TcpSocketPool::create(m_reactor); 
            if (NULL == ts) {
                close(fd);
                continue;
            }

            if (false == ts->start(fd, client_ip, m_ip, m_timeout)) {
                TcpSocketPool::remove(ts);
                continue;
            }

            LOG4CPLUS_INFO(log4cplus::Logger::getRoot(), "accept a new socket fd="<<fd);
        } else {

            if (errno == EAGAIN || errno == EINTR || errno == EMFILE || errno == ENFILE) {
                return true;
            }
            else {
                return false;
            }
        }
    }

};

bool TcpAccept::handle_output() {

    return false;
}

void TcpAccept::handle_error() {

    if (-1 == m_fd) {
        return;
    }

    m_reactor.set_handler(m_fd, NULL);
    m_reactor.handle_ctl(m_fd, EPOLL_CTL_DEL, 0);
    close(m_fd);
    m_fd = -1;
}

bool TcpAccept::push_buf(BufBlock_t* block) {
    //BufPool::free(block);
    free_block(block);
    return true;
}
