#pragma once
#include "core/foundation/misc.h"
#include <functional>
#include <thread>

namespace luna
{
class CORE_API LThread
{
public:
	LThread(std::function<void()> func);
	LThread();
	~LThread();
	void Join();
private:
	std::thread m_thread;
};

class ThreadManager : public Singleton<ThreadManager>
{
public:
	virtual ~ThreadManager()
	{
	}
};
}