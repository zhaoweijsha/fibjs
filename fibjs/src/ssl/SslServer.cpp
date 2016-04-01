/*
 * SslServer.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#include "SslServer.h"
#include "JSHandler.h"

namespace fibjs
{

result_t SslServer_base::_new(v8::Local<v8::Array> certs, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal,
                              v8::Local<v8::Object> This)
{
    return _new(certs, "", port, listener, retVal, This);
}

result_t SslServer_base::_new(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal,
                              v8::Local<v8::Object> This)
{
    obj_ptr<SslServer> svr = new SslServer();
    svr->wrap(This);

    result_t hr = svr->create(certs, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t SslServer_base::_new(X509Cert_base *crt, PKey_base *key, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal,
                              v8::Local<v8::Object> This)
{
    return _new(crt, key, "", port, listener, retVal, This);
}

result_t SslServer_base::_new(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal,
                              v8::Local<v8::Object> This)
{
    obj_ptr<SslServer> svr = new SslServer();
    svr->wrap(This);

    result_t hr = svr->create(crt, key, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t SslServer::create(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                           v8::Local<v8::Value> listener)
{
    result_t hr;
    obj_ptr<TcpServer_base> _server;
    obj_ptr<SslHandler_base> _handler;

    hr = SslHandler_base::_new(crt, key, listener, _handler);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, _handler->wrap(), _server);
    if (hr < 0)
        return hr;

    v8::Local<v8::Object> o = wrap();
    Isolate* isolate = holder();

    o->SetHiddenValue(isolate->NewFromUtf8("handler"), _handler->wrap());
    o->SetHiddenValue(isolate->NewFromUtf8("server"), _server->wrap());

    return 0;
}

result_t SslServer::create(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                           v8::Local<v8::Value> listener)
{
    result_t hr;
    obj_ptr<TcpServer_base> _server;
    obj_ptr<SslHandler_base> _handler;

    hr = SslHandler_base::_new(certs, listener, _handler);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, _handler->wrap(), _server);
    if (hr < 0)
        return hr;

    v8::Local<v8::Object> o = wrap();
    Isolate* isolate = holder();

    o->SetHiddenValue(isolate->NewFromUtf8("handler"), _handler->wrap());
    o->SetHiddenValue(isolate->NewFromUtf8("server"), _server->wrap());

    return 0;
}

result_t SslServer::run(AsyncEvent *ac)
{
    return server()->run(ac);
}

result_t SslServer::asyncRun()
{
    return server()->asyncRun();
}

result_t SslServer::stop(AsyncEvent *ac)
{
    return server()->stop(ac);
}

result_t SslServer::get_socket(obj_ptr<Socket_base> &retVal)
{
    return server()->get_socket(retVal);
}

result_t SslServer::get_handler(obj_ptr<Handler_base> &retVal)
{
    return handler()->get_handler(retVal);
}

result_t SslServer::set_handler(Handler_base *newVal)
{
    return handler()->set_handler(newVal);
}

result_t SslServer::get_verification(int32_t &retVal)
{
    return handler()->get_verification(retVal);
}

result_t SslServer::set_verification(int32_t newVal)
{
    return handler()->set_verification(newVal);
}

result_t SslServer::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    return handler()->get_ca(retVal);
}

result_t SslServer::get_stats(obj_ptr<Stats_base> &retVal)
{
    return server()->get_stats(retVal);
}

}
