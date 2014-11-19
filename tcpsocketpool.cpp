#include "tcpsocketpool.hpp"

TcpSocketPool TcpSocketPool::m_singleton;

TcpSocketPool::TcpSocketPool(){
};

TcpSocketPool::~TcpSocketPool(){
};

TcpSocket* TcpSocketPool::create(Reactor &reactor) {
    return m_singleton.m_socketPool.construct(reactor);
}

void TcpSocketPool::remove(TcpSocket* socket)
{
    return m_singleton.m_socketPool.destroy(socket);
}
