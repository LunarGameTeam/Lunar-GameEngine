#include "Core/Platform/thread.h"

namespace luna
{
LThread::LThread(std::function<void()> func) :
	m_thread(func)
{
}

LThread::~LThread()
{
	m_thread.detach();
}

void LThread::Join()
{
	m_thread.join();
}
}