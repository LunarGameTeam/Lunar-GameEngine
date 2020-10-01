#pragma once

#include "core/private_core.h"


namespace LunarEngine
{
	class LPathUtil
	{

	public:
		static void DivideFilePath(const LString& fullPath, LString& filePath, LString& fileName, LString& fileExt);

	};
}

