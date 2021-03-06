/*
 * Routing.h
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "ifs/Routing.h"
#include <pcre/pcre.h>
#include <vector>

#ifndef ROUTING_H_
#define ROUTING_H_

namespace fibjs
{

class Routing: public Routing_base
{
public:
    class rule: public obj_base
    {
    public:
        rule(pcre *re, pcre_extra *extra, Handler_base *hdlr) :
            m_re(re), m_extra(extra), m_hdlr(hdlr)
        {
        }

        ~rule()
        {
            pcre_free(m_re);
            pcre_free_study(m_extra);
        }

    public:
        pcre *m_re;
        pcre_extra *m_extra;
        naked_ptr<Handler_base> m_hdlr;
    };

public:
    // object_base
    virtual result_t dispose()
    {
        int32_t i, sz = (int32_t)m_array.size();

        for (i = 0; i < sz; i ++)
            m_array[i]->m_hdlr.dispose();

        return 0;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v,
                            obj_ptr<Handler_base> &retVal, AsyncEvent *ac);

public:
    // Routing_base
    virtual result_t append(v8::Local<v8::Object> map);
    virtual result_t append(const char *pattern, v8::Local<v8::Value> hdlr);

public:
    result_t append(const char *pattern, Handler_base *hdlr);

private:
    std::vector<obj_ptr<rule> > m_array;
};

} /* namespace fibjs */
#endif /* ROUTING_H_ */
