/*
 * os_posix.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#ifndef _WIN32

#include "ifs/os.h"
#include "ifs/process.h"
#include <sys/utsname.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <string.h>

namespace fibjs
{

result_t os_base::get_type(std::string &retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.sysname;

    return 0;
}

result_t os_base::get_version(std::string &retVal)
{
    struct utsname info;

    uname(&info);
    retVal = info.release;

    return 0;
}

result_t os_base::get_EOL(std::string& retVal)
{
    retVal.assign("\n", 1);
    return 0;
}

result_t os_base::networkInfo(v8::Local<v8::Object> &retVal)
{
    struct ::ifaddrs *addrs, *ent;
    struct ::sockaddr_in *in4;
    struct ::sockaddr_in6 *in6;
    char ip[INET6_ADDRSTRLEN];

    if (getifaddrs(&addrs) != 0)
        return CHECK_ERROR(LastError());

    Isolate* isolate = Isolate::current();

    retVal = v8::Object::New(isolate->m_isolate);

    for (ent = addrs; ent != NULL; ent = ent->ifa_next)
    {
        v8::Local<v8::Array> ret;
        v8::Local<v8::Object> o;
        v8::Local<v8::String> name, ipaddr, family;

        bzero(&ip, sizeof(ip));
        if (!(ent->ifa_flags & IFF_UP && ent->ifa_flags & IFF_RUNNING))
            continue;

        if (ent->ifa_addr == NULL)
            continue;

        if (ent->ifa_addr->sa_family != AF_INET6
                && ent->ifa_addr->sa_family != AF_INET)
            continue;

        name = isolate->NewFromUtf8(ent->ifa_name);
        if (retVal->Has(name))
        {
            ret = v8::Local<v8::Array>::Cast(retVal->Get(name));
        }
        else
        {
            ret = v8::Array::New(isolate->m_isolate);
            retVal->Set(name, ret);
        }

        if (ent->ifa_addr->sa_family == AF_INET6)
        {
            in6 = (struct sockaddr_in6 *) ent->ifa_addr;
            inet_ntop(AF_INET6, &(in6->sin6_addr), ip, INET6_ADDRSTRLEN);
            family = isolate->NewFromUtf8("IPv6");
        }
        else if (ent->ifa_addr->sa_family == AF_INET)
        {
            in4 = (struct sockaddr_in *) ent->ifa_addr;
            inet_ntop(AF_INET, &(in4->sin_addr), ip, INET6_ADDRSTRLEN);
            family = isolate->NewFromUtf8("IPv4");
        }

        o = v8::Object::New(isolate->m_isolate);
        o->Set(isolate->NewFromUtf8("address"), isolate->NewFromUtf8(ip));
        o->Set(isolate->NewFromUtf8("family"), family);
        o->Set(isolate->NewFromUtf8("internal"),
               ent->ifa_flags & IFF_LOOPBACK ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate));

        ret->Set(ret->Length(), o);
    }

    freeifaddrs(addrs);

    return 0;
}

result_t process_base::cwd(std::string &retVal)
{

#ifdef PATH_MAX
    char buf[PATH_MAX];
#else
    char buf[4096];
#endif

    if (!::getcwd(buf, sizeof(buf)))
        return CHECK_ERROR(LastError());

    retVal = buf;
    return 0;
}

result_t process_base::chdir(const char *directory)
{
    if (::chdir(directory))
        return CHECK_ERROR(LastError());

    return 0;
}

}

#endif
