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

    inline int pipe_r_fd() { return m_pipe[0]; }
    inline int pipe_s_fd() { return m_pipe[1]; }

private:
    T* m_qlist;
    int m_front;
    int m_rear;
    int m_size;

    int m_pipe[2];
};

#endif
