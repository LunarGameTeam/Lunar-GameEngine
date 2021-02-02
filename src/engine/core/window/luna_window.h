/*!
 * \file lunar_window.h
 * \date 2020/08/09 11:16
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

namespace luna
{

class CORE_API LWindow
{
public:
	virtual bool Init() = 0;


	inline int32_t GetWindowWidth()
	{
		return m_width;
	}
	inline int32_t GetWindowHeight()
	{
		return m_height;
	}
protected:
	bool m_full_screen = false;
	luna::LString m_window_name = "LunarGame-Engine";
	int32_t m_width = 1024;
	int32_t m_height = 768;
};


}