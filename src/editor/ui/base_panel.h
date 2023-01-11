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



namespace luna::editor
{


class PanelBase : public LObject
{
	RegisterTypeEmbedd(PanelBase, LObject)
public:
	PanelBase()
	{
	}
	~PanelBase()
	{

	}
	virtual void DoIMGUI();

	virtual void Init()
	{
	}

protected:
	virtual void OnGUI();


	float   mWidth  = 100;
	float   mHeight = 100;
	bool    mFocus  = false;
	bool    mInited = false;
	LString mTitle  = "Editor";
	bool	mHasMenu = false;

	friend class EditorModule;


};
}
