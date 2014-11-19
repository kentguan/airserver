#ifndef LOCK_HPP
#define LOCK_HPP

#include <pthread.h>

class MutexLock {
public:
    MutexLock();
    ~MutexLock();

public:
    void lock();
    void unlock();

private:
    pthread_mutex_t m_mutex_lock;
};

#endif
