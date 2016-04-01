/*
 * Isolate.h
 *
 *  Created on: May 2, 2015
 *      Author: lion
 */

#ifndef ISOLATE_H_
#define ISOLATE_H_

#include <exlib/include/list.h>
#include <exlib/include/service.h>
#include "QuickArray.h"

namespace fibjs
{

class SandBox;
class JSFiber;

class Isolate : public exlib::linkitem
{
public:
	class rt_base
	{
	public:
		rt_base()
		{
			m_isolate = Isolate::current();
		}

		~rt_base()
		{
			if (m_isolate->m_interrupt)
			{
				m_isolate->m_interrupt = false;
				m_isolate->InterruptCallback();
			}
		}

	protected:
		Isolate* m_isolate;
	};

	class rt : public rt_base
	{
	public:
		rt();
		~rt();

	public:
		static bool g_trace;

	private:
		JSFiber *m_fiber;
		v8::Unlocker unlocker;
	};

public:
	Isolate(const char *fname);

public:
	static Isolate *current();

	static bool check()
	{
		return current() != NULL;
	}

	void init();
	void InterruptCallback();

	v8::Local<v8::String> NewFromUtf8(const char* data, int length = -1)
	{
		return v8::String::NewFromUtf8(m_isolate, data, v8::String::kNormalString, length);
	}

	v8::Local<v8::String> NewFromUtf8(std::string str)
	{
		return v8::String::NewFromUtf8(m_isolate, str.c_str(), v8::String::kNormalString, (int32_t)str.length());
	}

public:
	int32_t m_id;
	std::string m_fname;

	QuickArray<void*> m_classInfo;

	exlib::spinlock m_weakLock;
	exlib::List<exlib::linkitem> m_weak;

	v8::Isolate *m_isolate;
	v8::Persistent<v8::Context> m_context;
	v8::Persistent<v8::Object> m_global;

	v8::Persistent<v8::Value> m_proto;
	v8::Persistent<v8::Object> m_json;
	v8::Persistent<v8::Function> m_stringify;

	obj_ptr<SandBox> m_topSandbox;

	exlib::List<exlib::linkitem> m_fibers;

	bool m_test_setup_bbd, m_test_setup_tdd;
	void* m_test;

	exlib::Queue<exlib::linkitem> m_jobs;
	int32_t m_currentFibers;
	int32_t m_idleFibers;

	int32_t m_loglevel;

	bool m_interrupt;
};

} /* namespace fibjs */
#endif /* ISOLATE_H_ */
