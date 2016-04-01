/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SubProcess_base_H_
#define _SubProcess_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "BufferedStream.h"

namespace fibjs
{

class BufferedStream_base;

class SubProcess_base : public BufferedStream_base
{
    DECLARE_CLASS(SubProcess_base);

public:
    // SubProcess_base
    virtual result_t kill(int32_t signal) = 0;
    virtual result_t wait(int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_pid(int32_t& retVal) = 0;
    virtual result_t get_stdin(obj_ptr<BufferedStream_base>& retVal) = 0;
    virtual result_t get_stdout(obj_ptr<BufferedStream_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_kill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_pid(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_stdin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_stdout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
    ASYNC_MEMBERVALUE1(SubProcess_base, wait, int32_t);
};

}


namespace fibjs
{
    inline ClassInfo& SubProcess_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"kill", s_kill, false},
            {"wait", s_wait, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"pid", s_get_pid, block_set, false},
            {"stdin", s_get_stdin, block_set, false},
            {"stdout", s_get_stdout, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "SubProcess", s__new, NULL, 
            2, s_method, 0, NULL, 3, s_property, NULL, NULL,
            &BufferedStream_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void SubProcess_base::s_get_pid(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(SubProcess_base);

        hr = pInst->get_pid(vr);

        METHOD_RETURN();
    }

    inline void SubProcess_base::s_get_stdin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<BufferedStream_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(SubProcess_base);

        hr = pInst->get_stdin(vr);

        METHOD_RETURN();
    }

    inline void SubProcess_base::s_get_stdout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<BufferedStream_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(SubProcess_base);

        hr = pInst->get_stdout(vr);

        METHOD_RETURN();
    }

    inline void SubProcess_base::s_kill(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(SubProcess_base);
        METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->kill(v0);

        METHOD_VOID();
    }

    inline void SubProcess_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(SubProcess_base);
        METHOD_ENTER(0, 0);

        hr = pInst->ac_wait(vr);

        METHOD_RETURN();
    }

}

#endif

