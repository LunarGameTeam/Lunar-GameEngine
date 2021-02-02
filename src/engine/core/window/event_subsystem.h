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
#include "luna_window.h"

namespace luna
{

enum class KeyCode : int
{
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
enum class EventType
{
	Input_KeyDown,
	Input_KeyUp,
	Input_KeyRelease,
	Input_MouseMove,
	Input_MousePress,
	Input_MouseRelease,
	Window_Resize,

};

struct LEvent
{
	EventType type;
};

struct CORE_API InputEvent : public LEvent
{
	EventType type;
	float x;
	float y;
	KeyCode code;
};

using InputCallback = boost::function<void(LWindow&, InputEvent&)>;

class CORE_API EventSubsystem : public SubSystem
{
public:
	bool OnPreInit() override;
	bool OnPostInit() override;
	bool OnInit() override;
	bool OnShutdown() override;
	void Tick(float delta_time) override;


	void OnInput(LWindow &window, InputEvent &evebt);
	void RegisterInputCallback(InputCallback callback);

private:
	InputCallback m_input_callback;
};

}