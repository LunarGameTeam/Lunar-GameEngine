/*!
 * \file render_subsystem.h
 * \date 2020/08/09 10:39
 *
 * \author IsakWong
 * Contact: isakwong@outlook.com
 *
 * \brief RenderSubSystem，渲染子模块，执行了渲染流程
 *
 * TODO: long description
 *
 * \note
*/
#pragma once

#include "core/core_module.h"
#include "DirectX12/PancySceneDesign.h"
class RenderSubusystem : public SubSystem
{
	SceneRoot* new_scene;
public:
	bool OnPreInit() override;


	bool OnPostInit() override;


	bool OnInit() override;


	bool OnShutdown() override;


	void Tick() override;

};