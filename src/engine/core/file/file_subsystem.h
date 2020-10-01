#pragma once

#include "core/subsystem/sub_system.h"
#include "platform_file.h"

namespace LunarEngine
{

	class FileSubsystem : public SubSystem
	{

	public:
		bool OnPreInit() override;


		bool OnPostInit() override;


		bool OnInit() override;


		bool OnShutdown() override;


		void Tick() override;

		IPlatformFileManager* GetPlatformFileManager() const {
			return m_PlatformFile;
		}
	private:
		IPlatformFileManager* m_PlatformFile;

	};
}