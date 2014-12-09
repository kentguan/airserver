
#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>                                                                                                          

#include "tcpsocket.hpp"
#include "reactor.hpp"
#include "global_data.hpp"
#include "tcpsocketpool.hpp"

//8192 socket 默认接受缓冲区大小 默认接受低水位1 发送高水位2048 
TcpSocket::TcpSocket(Reactor &reactor)
 :m_reactor(reactor), m_fd(-1), m_sendbufs(1024), m_recvbuf_size(8192) {

    m_recvpos = 0;
    m_sendpos = 0;
    m_recvbuf = (uint8_t *) malloc(m_recvbuf_size);
}

TcpSocket::~TcpSocket() {
    free(m_recvbuf);
}

bool TcpSocket::start(int fd, sockaddr_in &ip, sockaddr_in &local_ip, uint32_t conn_timeout) {
    m_fd = fd;
    m_local_ip = local_ip;
    m_ip = ip;

    m_conn_timeout = conn_timeout;
    m_last_time = m_reactor.get_curtime();

    int f;
    f = fcntl(m_fd, F_GETFL, 0);
    f |= O_NONBLOCK;
    fcntl(m_fd, F_SETFL, f);


    if (false == m_reactor.set_handler(m_fd, this)) {
        return false;
    }

    if (false == m_reactor.handle_ctl(m_fd, EPOLL_CTL_ADD, EPOLLIN|EPOLLET)) {
        return false;
    }

    m_id = m_reactor.make_id();
    m_sk.socket = fd;
    m_sk.recvtm = 0;
    m_sk.sendtm = 0;
    m_sk.local_ip = m_ip.sin_addr.s_addr;
    m_sk.local_port = m_ip.sin_port;
    m_sk.remote_ip = m_local_ip.sin_addr.s_addr;
    m_sk.remote_port = m_local_ip.sin_port;

    return true;
}


bool TcpSocket::handle_input() {

    bool recv_again = false;
    int len = 0;
    uint32_t packpos = 0;
    int n;

    //边缘模式 要读完全部数据
RECV:
    n = recv(m_fd, m_recvbuf + m_recvpos, m_recvbuf_size - m_recvpos, 0); 
    if (n > 0) {
        m_recvpos += n;

        recv_again = true;
        //if (m_recvpos == m_recvbuf_size) { //要处理完读取的数据，再去度新数据
            //recv_again = true;
        //}
        //else {
            //goto RECV;
        //}
    }
    else {
        if (n == 0) {
            return false; //对端关闭
        }
        else if (errno == EINTR) { //中断继续读
            goto RECV;
        }
        else if (errno == EAGAIN) {//缓冲区数据读完 
            return true;
        }
        else {
            return false;
        }
    }

READ:
    len = dll.handle_input((char*)m_recvbuf + packpos, m_recvpos - packpos, &m_sk);
    
    if (len < 0) { //包长度 出错
        return false;
    }
    else if (len == 0) { //不能分析出 包的长度

        if (packpos != 0) {
            goto MOVE;
        }

        if (m_recvpos == m_recvbuf_size) { //包头长度过长
            return false;
        }

        if (recv_again) {
            goto RECV;
        }
    }
    else {
        if (packpos == 0 && len >  m_recvbuf_size) { //包长度大于接受缓冲区
            uint8_t* tmp = (uint8_t*) realloc(m_recvbuf, len);
            if (tmp == NULL) {
                return false;
            }

            m_recvbuf = tmp;
            m_recvbuf_size = len;

            if (recv_again) {
                goto RECV;
            }
        }

        if (len + packpos <= m_recvpos) {//接受到一个完整的包
            //加入接受队列中
            //BufBlock_t* new_buf = BufPool::alloc(len);
            BufBlock_t* new_buf = alloc_block(len);
            memcpy(new_buf->page_base, m_recvbuf+packpos, len);
            new_buf->buf_head.id = m_id;
            new_buf->buf_head.buf_type = DATA_BLOCK;
            new_buf->buf_head.len = len;
            new_buf->buf_head.key = -1; //todo
            new_buf->buf_head.sk = m_sk;

            //加入到接受队列中
            while (1) {
                if (g_receive_queue.push_queue(new_buf) != 0) {
                    usleep(1);
                }
                else {
                    break;
                }
            }

            packpos += len;
            if (packpos < m_recvpos) {//还有一部分包 继续解析
                goto READ;
            }
        }
    }

MOVE:
    if (packpos != 0) { //解析出包，清除接受缓冲区中对应部分
        memcpy(m_recvbuf, m_recvbuf+packpos, m_recvpos - packpos);
        m_recvpos -= packpos;
        packpos = 0;
    }

    if (recv_again) {
        goto RECV;
    }

    return true;
}

bool TcpSocket::handle_output() {

    m_last_time = m_reactor.get_curtime();

    int n;

    while (!m_sendbufs.empty()) {
        BufBlock_t* block = m_sendbufs.front();

        if (block->buf_head.buf_type == FIN_BLOCK) {
            return false;
        }

        if (!(block->buf_head.buf_type & DATA_BLOCK)) {
            //BufPool::free(block);
            free_block(block);
            m_sendbufs.pop_front();
            continue;
        }

        while (1) {
            int n = send(m_fd, block->page_base + m_sendpos, block->buf_head.len - m_sendpos, 0);
            if (n > 0) {
                m_sendpos += n;
                
                if (m_sendpos == block->buf_head.len) {

                    if (block->buf_head.buf_type & FIN_BLOCK) {
                        return false;
                    }

                    //BufPool::free(block);
                    free_block(block);
                    m_sendbufs.pop_front();
                    m_sendpos = 0;
                    break;
                }

            }
            else {
                if (0 == n) { //对方已关闭
                    return false;
                }
                else if (errno == EAGAIN || errno == EINTR) { //EINTR 慢系统调用被信号中断 EAGAIN 继续写
                    continue;
                }

                return false;
            }
        }
    }

    m_reactor.handle_ctl(m_fd, EPOLL_CTL_MOD, EPOLLIN|EPOLLET); 

    return true;
};

void TcpSocket::handle_error() {

    if (m_fd != -1) {
        m_reactor.set_handler(m_fd, NULL);
        m_reactor.handle_ctl(m_fd, EPOLL_CTL_DEL, 0);
        close(m_fd);
        m_fd = -1;
    }

    while (!m_sendbufs.empty()) {
        BufBlock_t* block = m_sendbufs.front();
        //BufPool::free(block);
        free_block(block);
        m_sendbufs.pop_front();
    }

    TcpSocketPool::remove(this);
}

bool TcpSocket::push_buf(BufBlock_t* block) {

    if (m_sendbufs.full()) {
        return false;
    }

    m_sendbufs.push_back(block);
    m_reactor.handle_ctl(m_fd, EPOLL_CTL_MOD, EPOLLOUT|EPOLLIN|EPOLLET); 

    return true;
}


void TcpSocket::check_timeout() {

    if (m_conn_timeout == 0) {
        return;
    }

    if (m_last_time + m_conn_timeout < m_reactor.get_curtime()) {
        handle_error();
    }
}
