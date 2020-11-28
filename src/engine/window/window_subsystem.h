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
#include "lunar_window.h"

class WindowSubusystem : public SubSystem
{

public:

	LunarWindow* CreateLunarWindow(const LunarEngine::LString& name, int width, int height);
	LunarWindow* GetMainWindow();


public:
	bool OnPreInit() override;


	bool OnPostInit() override;


	bool OnInit() override;


	bool OnShutdown() override;

	void Tick() override;

private:
	LunarWindow* mMainWindow;
};