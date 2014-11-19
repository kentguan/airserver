#ifndef REACTOR_H
#define REACTOR_H

#include <time.h>
#include <stdint.h>
#include "handler.hpp"

//class HandlerBase;
class Reactor
{
public:
    Reactor();
    ~Reactor();

public:
    bool init(int size);
    bool fini();
    void handle_events(int time_out);

    bool set_handler(int fd, HandlerBase* handler);
    HandlerBase* get_handler(int fd);
    bool handle_ctl(int fd, int op, uint32_t events_mask);

    inline int handle_size() {return m_size;}
    inline int make_id() {return m_id++;}
    inline time_t get_curtime() {return m_curtime;}

private:
    int m_size;
    HandlerBase** m_handlers;
    bool m_inited;
    int m_fd;
    int m_handler_cnt;
    int m_id;
    time_t m_oldtime;
    time_t m_curtime;
};

#endif
