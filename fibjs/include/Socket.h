/*
 * Socket.h
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "ifs/Socket.h"
#include "Stream.h"
#include "inetAddr.h"
#include "AsyncIO.h"

#ifndef SOCKET_H_
#define SOCKET_H_

namespace fibjs
{

class Socket: public Socket_base
{
    FIBER_FREE();

public:
    Socket() :
        m_aio(INVALID_SOCKET, net_base::_AF_INET, net_base::_SOCK_STREAM)
#ifdef _WIN32
        , m_bBind(FALSE)
#endif
    {
    }

    Socket(SOCKET s, int32_t family, int32_t type) :
        m_aio(s, family, type)
#ifdef _WIN32
        , m_bBind(FALSE)
#endif
    {
    }

    virtual ~Socket();

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                          AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, AsyncEvent *ac);
    virtual result_t close(AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes,
                            int64_t &retVal, AsyncEvent *ac);

public:
    // Socket_base
    virtual result_t get_family(int32_t &retVal);
    virtual result_t get_type(int32_t &retVal);
    virtual result_t get_remoteAddress(std::string &retVal);
    virtual result_t get_remotePort(int32_t &retVal);
    virtual result_t get_localAddress(std::string &retVal);
    virtual result_t get_localPort(int32_t &retVal);
    virtual result_t connect(const char *host, int32_t port,
                             AsyncEvent *ac);
    virtual result_t bind(const char *addr, int32_t port, bool allowIPv4);
    virtual result_t bind(int32_t port, bool allowIPv4);
    virtual result_t listen(int32_t backlog, AsyncEvent* ac);
    virtual result_t accept(obj_ptr<Socket_base> &retVal,
                            AsyncEvent *ac);
    virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                          AsyncEvent *ac);
    virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base> &retVal);
    virtual result_t send(Buffer_base *data, AsyncEvent *ac);
    virtual result_t sendto(Buffer_base *data, const char *host,
                            int32_t port);

public:
    result_t create(int32_t family, int32_t type);

private:
    AsyncIO m_aio;
#ifdef _WIN32
    BOOL m_bBind;
#endif

    friend class AsyncIO;
};

}

#endif /* SOCKET_H_ */
