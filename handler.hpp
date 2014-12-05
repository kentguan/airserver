#ifndef HANDLER_H
#define HANDLER_H

//#include "buff.hpp"
#include "singleton_buff.hpp"

class HandlerBase {
public:
    virtual bool handle_input() = 0;
    virtual bool handle_output() = 0;
    virtual void handle_error() = 0;
    virtual bool push_buf(BufBlock_t* block) = 0;
    virtual void check_timeout() {};
    virtual ~HandlerBase() {}
};

#endif
