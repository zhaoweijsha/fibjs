/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _ZipInfo_base_H_
#define _ZipInfo_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class ZipInfo_base : public object_base
{
    DECLARE_CLASS(ZipInfo_base);

public:
    // ZipInfo_base
    virtual result_t get_filename(std::string& retVal) = 0;
    virtual result_t get_date(date_t& retVal) = 0;
    virtual result_t get_compress_type(std::string& retVal) = 0;
    virtual result_t get_compress_size(int64_t& retVal) = 0;
    virtual result_t get_file_size(int64_t& retVal) = 0;
    virtual result_t get_password(bool& retVal) = 0;
    virtual result_t get_data(obj_ptr<Buffer_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_filename(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_date(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_compress_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_compress_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_file_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_password(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& ZipInfo_base::class_info()
    {
        static ClassData::ClassProperty s_property[] = 
        {
            {"filename", s_get_filename, block_set, false},
            {"date", s_get_date, block_set, false},
            {"compress_type", s_get_compress_type, block_set, false},
            {"compress_size", s_get_compress_size, block_set, false},
            {"file_size", s_get_file_size, block_set, false},
            {"password", s_get_password, block_set, false},
            {"data", s_get_data, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "ZipInfo", s__new, NULL, 
            0, NULL, 0, NULL, 7, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void ZipInfo_base::s_get_filename(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_filename(vr);

        METHOD_RETURN();
    }

    inline void ZipInfo_base::s_get_date(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        date_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_date(vr);

        METHOD_RETURN();
    }

    inline void ZipInfo_base::s_get_compress_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_compress_type(vr);

        METHOD_RETURN();
    }

    inline void ZipInfo_base::s_get_compress_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_compress_size(vr);

        METHOD_RETURN();
    }

    inline void ZipInfo_base::s_get_file_size(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_file_size(vr);

        METHOD_RETURN();
    }

    inline void ZipInfo_base::s_get_password(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_password(vr);

        METHOD_RETURN();
    }

    inline void ZipInfo_base::s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Buffer_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(ZipInfo_base);

        hr = pInst->get_data(vr);

        METHOD_RETURN();
    }

}

#endif

