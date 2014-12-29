
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
    m_size = size;
}

template <class T> 
Queue<T>::~Queue() {

    if (m_qlist) {
        delete []m_qlist;
    }

    m_front = 0;
    m_rear = 0;
    m_size = 0;
}

template <class T> 
int Queue<T>::push_queue(const T& item) {

    if ((m_rear+1)&(m_size-1) == m_front) {
        return 1;
    }

    m_qlist[m_rear] = item;
    m_rear = (m_rear + 1)&(m_size-1);

    return 0;
}

template <class T> 
T Queue<T>::pop_queue() {
    if (m_rear == m_front) {
        return NULL;
    }

    T temp = m_qlist[m_front];
    m_front = (m_front+1)&(m_size-1);

    return temp;
}

template <class T> 
T Queue<T>::front() {
    if (m_rear == m_front) {
        return NULL;
    }

    return m_qlist[m_front];
}

template <class T> 
int Queue<T>::size() {
    return (m_rear-m_front+m_size)&(m_size-1);
}

