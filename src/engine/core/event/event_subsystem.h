/*!
 * \file event_subsytem.h
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

#include "core/private_core.h"
#include "core/core_module.h"
#include "core/misc/container.h"
#include "core/misc/signal.h"

namespace luna
{

class LWindow;

enum class CORE_API EventType
{
	Input_KeyDown,
	Input_KeyUp,
	Input_KeyRepeat,
	Input_MouseMove,
	Input_MousePress,
	Input_MouseRelease,
	Window_Resize,
	Window_Close,	
	App_Init,
	App_Exit,
	App_Pause
};

struct CORE_API LEvent
{
	EventType type;
};

enum class CORE_API KeyCode : int
{
	MouseLeft,
	MouseRight,
	MouseMiddle,
	Num1,
	Num2,
	Num3,
	Num4,
	Num5,
	Num6,
	Num7,
	Num8,
	Num9,
	Num0,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Escape,
	Enter,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12
};

struct CORE_API InputEvent : public LEvent
{
	float x;
	float y;
	KeyCode code;
};

class CORE_API EventSubsystem : public SubSystem
{
public:
	SIGNAL(OnInput, LWindow&, InputEvent&)
	SIGNAL(OnAppInit)
	SIGNAL(OnAppExit)

public:
	EventSubsystem() {};
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;

public:
	void Input(LWindow &window, InputEvent &evebt);

};

}