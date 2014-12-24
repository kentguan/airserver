#ifndef GLOBAL_DATA_HPP
#define GLOBAL_DATA_HPP

#include "queue.cpp"
#include "lock.hpp"
#include "airapi.h"
#include "sem.hpp"

struct BufBlock_t;

extern Queue<BufBlock_t*> g_receive_queue;  //接受队列
extern Queue<BufBlock_t*> g_send_queue;     //发送队列

extern MutexLock g_receive_lock;
extern MutexLock g_send_lock;

extern Sem g_receive_sem;

extern dll_func_t dll;

extern ServerConfig_t  g_server_conf;


#endif
