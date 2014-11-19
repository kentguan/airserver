#ifndef TCPSOCKETPOOL_HPP
#define TCPSOCKETPOOL_HPP

#include "tcpsocket.hpp"
#include <boost/pool/object_pool.hpp>

class Reactor;

class TcpSocketPool {

private:
    TcpSocketPool();
    ~TcpSocketPool();

public:
    static TcpSocket* create(Reactor &reactor);
    static void remove(TcpSocket* socket);

private:
    static TcpSocketPool m_singleton;
    boost::object_pool<TcpSocket> m_socketPool;
};

#endif
