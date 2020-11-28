#pragma once

#include "core/private_core.h"


namespace luna
{
	class LPathUtil
	{

	public:
		static void DivideFilePath(const LString& fullPath, LString& filePath, LString& fileName, LString& fileExt);

	};
}

