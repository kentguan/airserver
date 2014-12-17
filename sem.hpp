#ifndef SEM_HPP
#define SEM_HPP

#include <semaphore.h>

class Sem {
public:
    Sem();
    ~Sem();

public:
    void wait();
    void post();

private:

    sem_t m_sem;
};

#endif
