#include "thread.h"

namespace luna
{

LThread::LThread(boost::function<void()> func) :
	m_thread(func)
{
}

void LThread::Join()
{
	m_thread.join();
}

}