#include "sem.hpp"

Sem::Sem() {
    sem_init(&m_sem, 0, 0);
}

Sem::~Sem() {
    sem_destroy(&m_sem);
}

void Sem::wait() {
    sem_wait(&m_sem);
}

void Sem::post() {
    sem_post(&m_sem);
}

