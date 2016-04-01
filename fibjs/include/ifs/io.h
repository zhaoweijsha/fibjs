/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _io_base_H_
#define _io_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class MemoryStream_base;
class BufferedStream_base;

class io_base : public object_base
{
    DECLARE_CLASS(io_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:

};

}

#include "MemoryStream.h"
#include "BufferedStream.h"

namespace fibjs
{
    inline ClassInfo& io_base::class_info()
    {
        static ClassData::ClassObject s_object[] = 
        {
            {"MemoryStream", MemoryStream_base::class_info},
            {"BufferedStream", BufferedStream_base::class_info}
        };

        static ClassData s_cd = 
        { 
            "io", s__new, NULL, 
            0, NULL, 2, s_object, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


}

#endif

