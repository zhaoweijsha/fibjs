/*
 * console_log.cpp
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#include "console.h"

namespace fibjs
{

static int32_t s_loglevel = console_base::_NOTSET;
std_logger* s_std;
stream_logger* s_stream;

#define MAX_LOGGER  10
static logger* s_logs[MAX_LOGGER];

void init_logger()
{
    s_std = new std_logger;
}

void asyncLog(int32_t priority, std::string msg)
{
    Isolate* isolate = Isolate::current();

    if (isolate && priority > isolate->m_loglevel)
        return;

    if (priority > s_loglevel)
        return;

    int32_t i;

    for (i = 0; i < MAX_LOGGER; i ++)
    {
        logger* lgr = s_logs[i];

        if (lgr)
            lgr->log(priority, msg);
        else
            break;
    }

    if (i == 0)
        s_std->log(priority, msg);

    if (s_stream)
        s_stream->log(priority, msg);
}

void flushLog(bool bFiber)
{
    int32_t i;

    for (i = 0; i < MAX_LOGGER; i ++)
    {
        logger* lgr = s_logs[i];

        if (lgr)
            lgr->flush(bFiber);
        else
            break;
    }

    s_std->flush(bFiber);
}

result_t console_base::get_loglevel(int32_t &retVal)
{
    retVal = s_loglevel;
    return 0;
}

result_t console_base::set_loglevel(int32_t newVal)
{
    s_loglevel = newVal;
    return 0;
}

result_t console_base::get_colors(obj_ptr<TextColor_base> &retVal)
{
    retVal = logger::get_std_color();
    return 0;
}

result_t console_base::add(v8::Local<v8::Value> cfg)
{
    int32_t n = 0;

    for (n = 0; n < MAX_LOGGER && s_logs[n]; n ++);

    if (n >= MAX_LOGGER)
        return CHECK_ERROR(Runtime::setError("Too many items."));

    v8::Local<v8::Value> type;
    v8::Local<v8::Object> o;
    Isolate* isolate = Isolate::current();

    if (cfg->IsString() || cfg->IsStringObject())
    {
        type = cfg;
        o = v8::Object::New(isolate->m_isolate);
    }
    else if (cfg->IsObject())
    {
        o = v8::Local<v8::Object>::Cast(cfg);
        type = o->Get( isolate->NewFromUtf8("type", 4));

        if (IsEmpty(type))
            return CHECK_ERROR(Runtime::setError("Missing log type."));
    }
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::String::Utf8Value s(type);
    if (!*s)
        return CHECK_ERROR(Runtime::setError("Unknown log type."));

    logger* lgr;

    if (!qstrcmp(*s, "console"))
        lgr = new std_logger();
#ifndef _WIN32
    else if (!qstrcmp(*s, "syslog"))
        lgr = new sys_logger();
#endif
    else if (!qstrcmp(*s, "file"))
        lgr = new file_logger();
    else
        return CHECK_ERROR(Runtime::setError("Unknown log type."));

    if (lgr)
    {
        result_t hr = lgr->config(isolate, o);
        if (hr < 0)
        {
            lgr->stop();
            return hr;
        }

        s_logs[n] = lgr;
    }

    return 0;
}

result_t console_base::add(v8::Local<v8::Array> cfg)
{
    int32_t sz = cfg->Length();
    int32_t i;
    result_t hr;

    for (i = 0; i < sz; i ++)
    {
        hr = add(cfg->Get(i));
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t console_base::reset()
{
    int32_t i;

    for (i = 0; i < MAX_LOGGER; i ++)
    {
        logger* lgr = s_logs[i];

        if (lgr)
        {
            lgr->stop();
            s_logs[i] = 0;
        }
        else
            break;
    }

    return 0;
}

}
