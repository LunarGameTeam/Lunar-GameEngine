#pragma once

#include "core/framework/module.h"
#include "platform_file.h"

namespace luna
{
/*!
 * \class FileSubsystem
 *
 *
 * \brief FileSubsystem 是平台无关的文件子系统，提供了文件操作Manager
 *
 *
 * \author IsakWong
 *
 * \version 1.0
 *
 *
 */
class CORE_API PlatformModule : public LModule
{
	RegisterTypeEmbedd(PlatformModule, LModule)
public:
	PlatformModule();;
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

	IPlatformFileManager *GetPlatformFileManager() const {
		return mPlatformFile;
	}

private:
	IPlatformFileManager *mPlatformFile;
};

CORE_API extern PlatformModule *sPlatformModule;
}