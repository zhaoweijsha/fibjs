/*
 * Fiber.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "ifs/coroutine.h"
#include "ifs/Fiber.h"
#include "QuickArray.h"

#ifndef FIBER_H_
#define FIBER_H_

namespace fibjs
{

class FiberBase: public Fiber_base,
    public AsyncEvent
{
    FIBER_FREE();

public:
    // Fiber_base
    virtual result_t join();
    virtual result_t get_traceInfo(std::string& retVal);
    virtual result_t get_caller(obj_ptr<Fiber_base> &retVal);

public:
    static void *fiber_proc(void *p);
    void start();

    void set_caller(Fiber_base* caller);

public:
    std::string m_traceInfo;
    exlib::Event m_quit;
    weak_ptr<Fiber_base> m_caller;
};

class JSFiber: public FiberBase
{
public:
    class scope
    {
    public:
        scope(JSFiber *fb = NULL);
        ~scope();

        JSFiber *operator->()
        {
            return m_pFiber;
        }

    public:
        result_t m_hr;

    private:
        obj_ptr<JSFiber> m_pFiber;
        TryCatch try_catch;
    };

public:
    ~JSFiber()
    {
        clear();
    }

    static JSFiber *current();
    virtual void js_invoke();

    template<typename T>
    void New(v8::Local<v8::Function> func, T &args, int32_t nArgStart,
             int32_t nArgCount)
    {
        Isolate* isolate = holder();
        int32_t i;

        m_argv.resize(nArgCount - nArgStart);
        for (i = nArgStart; i < nArgCount; i++)
            m_argv[i - nArgStart].Reset(isolate->m_isolate, args[i]);
        m_func.Reset(isolate->m_isolate, func);

        start();
    }

    template<typename T>
    static result_t New(v8::Local<v8::Function> func,
                        const v8::FunctionCallbackInfo<v8::Value> &args, int32_t nArgStart,
                        obj_ptr<T> &retVal)
    {
        obj_ptr<JSFiber> fb = new JSFiber();
        fb->New(func, args, nArgStart, args.Length());
        retVal = fb;

        return 0;
    }

    template<typename T>
    static result_t New(v8::Local<v8::Function> func,
                        v8::Local<v8::Value> *args, int32_t argCount, obj_ptr<T> &retVal)
    {
        obj_ptr<JSFiber> fb = new JSFiber();
        fb->New(func, args, 0, argCount);
        retVal = fb;

        return 0;
    }

    result_t get_result(v8::Local<v8::Value> &retVal)
    {
        if (m_result.IsEmpty())
            return CALL_RETURN_NULL;

        retVal = v8::Local<v8::Value>::New(holder()->m_isolate, m_result);
        return 0;
    }

    void clear()
    {
        size_t i;

        for (i = 0; i < m_argv.size(); i++)
            m_argv[i].Reset();
        m_argv.resize(0);

        m_func.Reset();
        m_result.Reset();
    }

private:
    v8::Persistent<v8::Function> m_func;
    QuickArray<v8::Persistent<v8::Value> > m_argv;
    v8::Persistent<v8::Value> m_result;
};

} /* namespace fibjs */
#endif /* FIBER_H_ */
