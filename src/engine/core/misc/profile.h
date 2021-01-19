#pragma once

#include "core/private_core.h"

class ProfileGuard
{
public:
	ProfileGuard() :_curTimePoint(std::chrono::steady_clock::now())
	{

	}
	void Reset()
	{
		auto curTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _curTimePoint);
		LogVerboseFormat(E_Core, "%d ms", duration.count());
		_curTimePoint = std::chrono::steady_clock::now();
	}
	~ProfileGuard()
	{
		auto curTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _curTimePoint);
		LogVerboseFormat(E_Core, "%d ms", duration.count());
	}

private:
	std::chrono::steady_clock::time_point _curTimePoint;
};
