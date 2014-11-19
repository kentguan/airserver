
#include <sys/epoll.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>                                                                                                             
#include <unistd.h>                                                                                                             
#include "reactor.hpp"                                                                                                             
Reactor::Reactor() {
    m_inited = false;
    m_size = 0;
    m_handlers = NULL;
    m_fd = 0;
    m_handler_cnt = 0;
    m_id = 0;
};

Reactor::~Reactor() {
};

bool Reactor::init(int size) {
    
    if (m_inited) {
        return false;
    }

    m_size = size;
    m_handlers = (HandlerBase**)calloc(sizeof(HandlerBase*), m_size);

    if (NULL == m_handlers) {
        return false;
    }

    for (int i = 0; i < m_size; i++) {
        m_handlers[i] = NULL;
    }

    m_fd = epoll_create(m_size);
    if (-1 == m_fd) {
        free(m_handlers);
        return false;
    }

    m_inited = true;
    return true;
};

bool Reactor::fini() {

    if (!m_inited) {
        return false;
    }

    for (int i = 0; i < m_size; i++) {
        if (m_handlers[i] != NULL) {
            m_handlers[i]->handle_error();
        }
    }

    free(m_handlers);
    close(m_fd);

    m_handlers = NULL;
}

bool Reactor::set_handler(int fd, HandlerBase* handler) {

    if (fd > m_size) {
        return false;
    }

    if (handler == NULL) {
        m_handler_cnt--;
    }
    else {
        m_handler_cnt++;
    }

    m_handlers[fd] = handler;

    return true;
}

void Reactor::handle_events(int timeout) {

    time(&m_curtime);
    struct epoll_event events[128];
    
    const int n = epoll_wait(m_fd, events, 128, timeout);
    for (int i = 0; i < n; i++) {

        uint32_t e = events[i].events;
        if (NULL == m_handlers[events[i].data.fd]) {
            continue;
        }
        else {
            if (e & (EPOLLERR|EPOLLHUP|EPOLLPRI)) {
                m_handlers[events[i].data.fd]->handle_error();
                continue;
            }

            if (e & EPOLLIN) {
                if (false == m_handlers[events[i].data.fd]->handle_input()) {
                    m_handlers[events[i].data.fd]->handle_error();
                }
            }

            if (e & EPOLLOUT) {
                if (false == m_handlers[events[i].data.fd]->handle_output()) {
                    m_handlers[events[i].data.fd]->handle_error();
                }
            }
        }
    }

    //触发定时事件
    if (m_curtime > m_oldtime) {
        int num = 0;
        int handle_cnt = m_handler_cnt;

        for (int i = 0; i < m_size; i++) {
            if (m_handlers[i] == NULL) {
                continue;
            }
            num++;
            m_handlers[i]->check_timeout();

            if (m_handler_cnt == num) {
                break;
            }
        }

        m_oldtime = m_curtime;
    }

}

bool Reactor::handle_ctl(int fd, int op, uint32_t events_mask) {

    if (fd > m_size || fd < 0) {
        return false;
    }

    epoll_event e;
    e.data.fd = fd;
    e.events = events_mask;
    if (-1 == epoll_ctl(m_fd, op, fd, &e)) {
        return false;
    }

    return true;
}

HandlerBase* Reactor::get_handler(int fd) {

    if (fd >m_size || fd < 0) {
        return NULL;
    }

    return m_handlers[fd];
}

