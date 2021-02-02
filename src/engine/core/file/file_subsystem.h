#pragma once

#include "core/subsystem/sub_system.h"
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
	class CORE_API FileSubsystem : public SubSystem
	{

	public:
		bool OnPreInit() override;


		bool OnPostInit() override;


		bool OnInit() override;


		bool OnShutdown() override;


		void Tick(float delta_time) override;

		IPlatformFileManager* GetPlatformFileManager() const {
			return m_PlatformFile;
		}
	private:
		IPlatformFileManager* m_PlatformFile;

	};
}