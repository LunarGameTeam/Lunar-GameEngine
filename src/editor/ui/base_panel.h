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

#include "core/core_library.h"
#include "core/reflection/reflection.h"

#include "imgui.h"


namespace luna::editor
{


bool InputLString(const char* label, LString* str, ImGuiInputTextFlags flags);


class EditorBase : public LObject
{
	RegisterTypeEmbedd(EditorBase, LObject)
public:
	EditorBase()
	{
	}
	~EditorBase()
	{

	}
	virtual void DoIMGUI();

	virtual void Init()
	{
	}

protected:
	virtual void OnGUI();

	bool CustomTreeNode(const char* id, ImGuiTreeNodeFlags flag, std::function<void(bool, bool)> func);

	float   mWidth  = 100;
	float   mHeight = 100;
	bool    mFocus  = false;
	bool    mInited = false;
	LString mTitle  = "Editor";

	friend class EditorModule;


};
}
