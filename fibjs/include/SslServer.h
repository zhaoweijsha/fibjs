/*
 * SslServer.h
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#ifndef _sj_SSLSERVER_H_
#define _sj_SSLSERVER_H_

#include "ifs/SslServer.h"
#include "TcpServer.h"
#include "SslHandler.h"

namespace fibjs
{

class SslServer: public SslServer_base
{
    FIBER_FREE();

public:
    // object_base
    virtual result_t dispose()
    {
        return 0;
    }

public:
    // TcpServer_base
    virtual result_t run(AsyncEvent *ac);
    virtual result_t asyncRun();
    virtual result_t stop(AsyncEvent *ac);
    virtual result_t get_socket(obj_ptr<Socket_base> &retVal);
    virtual result_t get_handler(obj_ptr<Handler_base> &retVal);
    virtual result_t set_handler(Handler_base *newVal);
    virtual result_t get_stats(obj_ptr<Stats_base> &retVal);

public:
    // SslServer_base
    virtual result_t get_verification(int32_t &retVal);
    virtual result_t set_verification(int32_t newVal);
    virtual result_t get_ca(obj_ptr<X509Cert_base> &retVal);

public:
    result_t create(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                    v8::Local<v8::Value> listener);
    result_t create(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                    v8::Local<v8::Value> listener);

private:
    TcpServer_base* server()
    {
        return TcpServer_base::getInstance(wrap()->GetHiddenValue(holder()->NewFromUtf8("server")));
    }

    SslHandler_base* handler()
    {
        return SslHandler_base::getInstance(wrap()->GetHiddenValue(holder()->NewFromUtf8("handler")));
    }
};

} /* namespace fibjs */

#endif /* _sj_SSLSERVER_H_ */
