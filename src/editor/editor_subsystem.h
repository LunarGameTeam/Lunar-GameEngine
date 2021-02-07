/*!
 * \file window_subsystem.h
 * \date 2020/08/09 11:14
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

#include "core/core_module.h"
#include "core/misc/container.h"
#include "editor_base.h"
#include "core/config/config.h"


namespace luna
{
	namespace editor
	{
		class EditorSubsystem : public SubSystem
		{
		public:
			EditorSubsystem() {};

		public:
			bool OnPreInit() override;
			bool OnPostInit() override;
			bool OnInit() override;
			bool OnShutdown() override;
			void Tick(float delta_time) override;
			void OnIMGUI();
		private:
			LVector<EditorBase*> m_editors;
		};
	}
}