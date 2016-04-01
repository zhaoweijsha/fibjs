/*
 * Runtime.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "utils.h"

#ifndef RUNTIME_H_
#define RUNTIME_H_

namespace fibjs
{

class Runtime
{
public:
    Runtime(Isolate* isolate) : m_isolate(isolate)
    {
        reg();
    }

public:
    static Runtime &current();

    static result_t setError(result_t hr)
    {
        Runtime &rt = Runtime::current();

        rt.m_code = hr;
        return rt.m_code;
    }

    static result_t setError(std::string err)
    {
        Runtime &rt = Runtime::current();

        rt.m_code = CALL_E_EXCEPTION;
        rt.m_error = err;
        return rt.m_code;
    }

    static result_t setError(const char *err = NULL)
    {
        Runtime &rt = Runtime::current();

        rt.m_code = CALL_E_EXCEPTION;
        rt.m_error.assign(err ? err : "");
        return rt.m_code;
    }

    static const std::string &errMessage()
    {
        return Runtime::current().m_error;
    }

    static result_t errNumber()
    {
        return Runtime::current().m_code;
    }

    Isolate* isolate()
    {
        return m_isolate;
    }

private:
    void reg();

private:
    result_t m_code;
    std::string m_error;
    Isolate* m_isolate;
};

} /* namespace fibjs */
#endif /* RUNTIME_H_ */
