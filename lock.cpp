#include "lock.hpp"

MutexLock::MutexLock() {
    pthread_mutex_init(&m_mutex_lock, NULL);
}

MutexLock::~MutexLock() {
    pthread_mutex_destroy(&m_mutex_lock);
}

void MutexLock::lock() {
    pthread_mutex_lock(&m_mutex_lock);
}

void MutexLock::unlock() {
    pthread_mutex_unlock(&m_mutex_lock);
}

