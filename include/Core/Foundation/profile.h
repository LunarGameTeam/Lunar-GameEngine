#pragma once


#include <chrono>

class CORE_API ScopedProfileGuard
{
public:
	ScopedProfileGuard(const luna::LString& str = "") :
		_curTimePoint(std::chrono::steady_clock::now()),
		_str(str)
	{

	}
	void Reset()
	{
		auto curTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _curTimePoint);
		
		Log("Core", "[{0:>4} ms] {1}", duration.count(), _str.c_str());
		_curTimePoint = std::chrono::steady_clock::now();
	}
	~ScopedProfileGuard()
	{
		auto curTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _curTimePoint);
		if (duration.count() > 1000)
		{
			LogWarning("Core", "[{0:>4} ms] {1}", duration.count(), _str.c_str());
		}else	
			Log("Core", "[{0:>4} ms] {1}", duration.count(), _str.c_str());
	}

private:
	luna::LString _str;
	std::chrono::steady_clock::time_point _curTimePoint;
};
