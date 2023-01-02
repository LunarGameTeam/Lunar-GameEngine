#pragma once

#include "core/framework/module.h"
#include "core/foundation/config.h"
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
CORE_API CONFIG_DECLARE(LString, Start, DefaultProject, "");

class CORE_API PlatformModule : public LModule
{
	RegisterTypeEmbedd(PlatformModule, LModule)
public:
	PlatformModule();;
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

	LString GetEngineDir();
	LString GetProjectDir();

	void SetProjectDir(const LString& val)
	{
		Config_DefaultProject.SetValue(val);
		mPlatformFile->SetProjectDir(val);
	}

	IPlatformFileManager *GetPlatformFileManager() const {
		return mPlatformFile;
	}

private:
	IPlatformFileManager *mPlatformFile;
};

CORE_API extern PlatformModule *sPlatformModule;
}