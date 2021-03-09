#pragma once
#include "core/misc/misc.h"
#include <boost/function.hpp>
#include <boost/thread.hpp>

namespace luna
{

class CORE_API LThread
{
public:
	LThread(boost::function<void()> func);
	LThread();
	void Join();
private:
	boost::thread m_thread;

};

class ThreadManager : public Singleton<ThreadManager>
{
public:
	virtual ~ThreadManager()
	{

	}
};

}