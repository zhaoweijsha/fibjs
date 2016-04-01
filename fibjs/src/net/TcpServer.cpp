/*
 * TcpServer.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "TcpServer.h"
#include "ifs/mq.h"
#include "JSHandler.h"
#include "ifs/console.h"

namespace fibjs
{

result_t _new_tcpServer(const char *addr, int32_t port,
                        Handler_base *listener, obj_ptr<TcpServer_base> &retVal,
                        v8::Local<v8::Object> This)
{
    obj_ptr<TcpServer> svr = new TcpServer();
    svr->wrap(This);

    result_t hr = svr->create(addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t TcpServer_base::_new(int32_t port, v8::Local<v8::Value> listener,
                              obj_ptr<TcpServer_base> &retVal,
                              v8::Local<v8::Object> This)
{
    return _new("", port, listener, retVal, This);
}

result_t TcpServer_base::_new(const char *addr, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<TcpServer_base> &retVal,
                              v8::Local<v8::Object> This)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr = JSHandler::New(listener, hdlr1);
    if (hr < 0)
        return hr;

    return _new_tcpServer(addr, port, hdlr1, retVal, This);
}

static const char *s_staticCounter[] =
{ "total", "connections" };
static const char *s_Counter[] =
{ "accept", "close" };

enum
{
    TCPS_TOTAL = 0, TCPS_CONNECTIONS, TCPS_ACCEPT, TCPS_CLOSE
};

TcpServer::TcpServer()
{
    m_stats = new Stats();
    m_stats->init(s_staticCounter, 2, s_Counter, 2);
    m_running = false;
}

result_t TcpServer::create(const char *addr, int32_t port,
                           Handler_base *listener)
{
    result_t hr;

    hr = Socket_base::_new(net_base::_AF_INET, net_base::_SOCK_STREAM,
                           m_socket);
    if (hr < 0)
        return hr;

    hr = m_socket->bind(addr, port, false);
    if (hr < 0)
        return hr;

    hr = m_socket->ac_listen(1024);
    if (hr < 0)
        return hr;

    set_handler(listener);

    return 0;
}

result_t TcpServer::run(AsyncEvent *ac)
{
    class asyncInvoke: public AsyncState
    {
    public:
        asyncInvoke(TcpServer *pThis, Socket_base *pSock) :
            AsyncState(NULL), m_pThis(pThis), m_sock(pSock), m_obj(pSock)
        {
            set(invoke);
        }

    public:
        static int32_t invoke(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->set(close);
            return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_obj, pThis);
        }

        static int32_t close(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;

            pThis->done();
            pThis->m_pThis->m_stats->inc(TCPS_CLOSE);
            pThis->m_pThis->m_stats->dec(TCPS_CONNECTIONS);
            return pThis->m_sock->close(pThis);
        }

        virtual int32_t error(int32_t v)
        {
            asyncLog(console_base::_ERROR, "TcpServer: " + getResultMessage(v));
            set(close);
            return 0;
        }

    private:
        obj_ptr<TcpServer> m_pThis;
        obj_ptr<Socket_base> m_sock;
        obj_ptr<object_base> m_obj;
    };

    class asyncAccept: public AsyncState
    {
    public:
        asyncAccept(TcpServer *pThis, AsyncEvent *ac) :
            AsyncState(ac), m_pThis(pThis)
        {
            set(accept);
        }

    public:
        static int32_t accept(AsyncState *pState, int32_t n)
        {
            asyncAccept *pThis = (asyncAccept *) pState;

            pThis->set(invoke);
            return pThis->m_pThis->m_socket->accept(pThis->m_retVal, pThis);
        }

        static int32_t invoke(AsyncState *pState, int32_t n)
        {
            asyncAccept *pThis = (asyncAccept *) pState;

            pThis->m_pThis->m_stats->inc(TCPS_TOTAL);
            pThis->m_pThis->m_stats->inc(TCPS_ACCEPT);
            pThis->m_pThis->m_stats->inc(TCPS_CONNECTIONS);

            if (pThis->m_retVal)
            {
                (new asyncInvoke(pThis->m_pThis, pThis->m_retVal))->apost(0);
                pThis->m_retVal.Release();
            }

            return pThis->m_pThis->m_socket->accept(pThis->m_retVal, pThis);
        }

        virtual int32_t error(int32_t v)
        {
            if (v == CALL_E_BAD_FILE || v == CALL_E_INVALID_CALL)
            {
                m_pThis->dispose();
                return v;
            }

            asyncLog(console_base::_ERROR, "TcpServer: " + getResultMessage(v));
            return 0;
        }

    private:
        obj_ptr<TcpServer> m_pThis;
        obj_ptr<Socket_base> m_retVal;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    m_running = true;

    return (new asyncAccept(this, ac))->post(0);
}

result_t TcpServer::asyncRun()
{
    class asyncCall: public AsyncState
    {
    public:
        asyncCall(TcpServer *pThis) :
            AsyncState(NULL), m_pThis(pThis)
        {
            set(accept);
        }

    public:
        static int32_t accept(AsyncState *pState, int32_t n)
        {
            asyncCall *pThis = (asyncCall *) pState;
            return pThis->m_pThis->run(pThis);
        }

    private:
        obj_ptr<TcpServer> m_pThis;
    };

    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_running)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    (new asyncCall(this))->apost(0);
    return 0;
}

result_t TcpServer::stop(AsyncEvent *ac)
{
    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return m_socket->close(ac);
}

result_t TcpServer::get_socket(obj_ptr<Socket_base> &retVal)
{
    if (!m_socket)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_socket;
    return 0;
}

result_t TcpServer::get_handler(obj_ptr<Handler_base> &retVal)
{
    retVal = m_hdlr;
    return 0;
}

result_t TcpServer::set_handler(Handler_base *newVal)
{
    obj_ptr<Handler_base> hdlr = (Handler_base*)m_hdlr;

    wrap()->SetHiddenValue(holder()->NewFromUtf8("handler"), newVal->wrap());
    m_hdlr = newVal;

    if (hdlr)
        hdlr->dispose();

    return 0;
}

result_t TcpServer::get_stats(obj_ptr<Stats_base> &retVal)
{
    retVal = m_stats;
    return 0;
}

} /* namespace fibjs */
