#pragma once

#include "Core/Framework/Module.h"
#include "Core/Foundation/Config.h"
#include "PlatformFile.h"
#include "Core/Event/EventModule.h"

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
CORE_API CONFIG_DECLARE(int, Window, UsingImGUI, 1);
CORE_API CONFIG_DECLARE(int, Window, DefaultWidth, 1024);
CORE_API CONFIG_DECLARE(int, Window, DefaultHeight, 768);

using WindowHandle = uint32_t;

class LWindow;

class CORE_API PlatformModule : public LModule
{
	RegisterTypeEmbedd(PlatformModule, LModule)
public:
	SIGNAL(OnWindowResize, LWindow&, WindowEvent&);

	PlatformModule();
	bool OnLoad() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;
	LWindow* CreateLunaWindow(const luna::LString& name, int width, int height);
	LWindow* GetMainWindow();
	LWindow* GetWindowByHandle(WindowHandle handle);


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
	IPlatformFileManager* mPlatformFile;
	LMap<WindowHandle, LWindow*> mWindows;
	LWindow* mMainWindow;
};

CORE_API extern PlatformModule *sPlatformModule;
}