
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "queue.hpp"

template <class T> 
Queue<T>::Queue(int size) {
    m_qlist =  new T[size];
    if (!m_qlist) {
        exit(1);
    }

    m_front = 0;
    m_rear = 0;
    m_count = 0;
    m_size = size;

    if (-1 == pipe(m_pipe)) {
        close(m_pipe[0]);
        close(m_pipe[1]);
    }

    int f = fcntl(m_pipe[1], F_GETFL, 0);
    f |= O_NONBLOCK; 
    fcntl(m_pipe[1], F_SETFL, f);
}

template <class T> 
Queue<T>::~Queue() {

    if (m_qlist) {
        delete []m_qlist;
    }

    m_front = 0;
    m_rear = 0;
    m_count = 0;
    m_size = 0;

    close(m_pipe[0]);
    close(m_pipe[1]);
}

template <class T> 
int Queue<T>::push_queue(const T& item) {

    if ((m_rear+1)%m_size == m_front) {
        return 1;
    }

    m_qlist[m_rear] = item;
    m_rear = (m_rear + 1)%m_size;
    m_count++;

#ifndef WORK_MODE
    write(m_pipe[1], "r", 1);
#endif

    return 0;
}

template <class T> 
T Queue<T>::pop_queue() {
    if (0 == m_count) {
        return NULL;
    }

    int temp = m_front;
    m_front = (m_front+1)%m_size;
    m_count--;

    return m_qlist[temp];
}

template <class T> 
T Queue<T>::front() {
    if (0 == m_count) {
        return NULL;
    }

    return m_qlist[m_front];
}

template <class T> 
int Queue<T>::size() {
    return m_count;
}

