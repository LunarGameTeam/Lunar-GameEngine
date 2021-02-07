#pragma once

#include "core/private_core.h"
#include "core/misc/string.h"


namespace luna
{
	class LPathUtil
	{

	public:
		static void DivideFilePath(const LString& fullPath, LString& filePath, LString& fileName, LString& fileExt);

	};
}

