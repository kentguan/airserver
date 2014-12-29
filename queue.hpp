#ifndef QUEUE_HPP
#define QUEUE_HPP

const int nDefaultQueueSize = 1<<23;
//const int nDefaultQueueSize = 4;

template <class T> class Queue {
public:
    Queue(int initsize=nDefaultQueueSize);
    ~Queue();

public:
    int push_queue(const T& item);
    T pop_queue();
    T front();
    int size();

private:
    T* m_qlist;
    int m_front;
    int m_rear;
    int m_size;
};

#endif
