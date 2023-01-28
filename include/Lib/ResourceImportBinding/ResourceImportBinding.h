#pragma once
#include "Core/CoreMin.h"
#include "Lib/ImportScene/ImportScene.h"
#ifndef RESOURCE_EXPORT_API
#ifdef RESOURCE_EXPORT
#define RESOURCE_EXPORT_API __declspec( dllexport )//宏定义
#else
#define RESOURCE_EXPORT_API __declspec( dllimport )
#endif
#endif
namespace luna
{
	void RESOURCE_EXPORT_API ImportFileToLunaResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension
	);
}