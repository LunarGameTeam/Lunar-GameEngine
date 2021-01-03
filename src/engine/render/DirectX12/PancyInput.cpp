#include"PancyInput.h"
#include "core/core_module.h"
PancyInput* PancyInput::pancy_input_pInstance = NULL;

using namespace luna;
PancyInput::PancyInput()
{
}
LResult PancyInput::Init(HWND hwnd, HINSTANCE hinst)
{
	HRESULT hr = DirectInput8Create(hinst, DIRECTINPUT_HEADER_VERSION, IID_IDirectInput8, (void**)&pancy_dinput, NULL);//获取DirectInput设备
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "init directinput error", error_message);

		return error_message;
	}
	LResult check_error = DinputClear(hwnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE), (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));//创建键盘及鼠标
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	return g_Succeed;
}
PancyInput::~PancyInput()
{
	pancy_dinput->Release();
	dinput_keyboard->Release();
	dinput_mouse->Release();
}
LResult PancyInput::DinputClear(HWND hwnd, DWORD keyboardCoopFlags, DWORD mouseCoopFlags)
{
	//创建键盘设备
	HRESULT hr = pancy_dinput->CreateDevice(GUID_SysKeyboard, &dinput_keyboard, NULL);
	if (FAILED(hr))
	{
		LResult error_message;
		LunarDebugLogError(hr, "init directinput device error", error_message);

		return error_message;
	}
	dinput_keyboard->SetDataFormat(&c_dfDIKeyboard);//设置设备的数据格式
	dinput_keyboard->SetCooperativeLevel(hwnd, keyboardCoopFlags);//设置设备的独占等级
	dinput_keyboard->Acquire();//获取设备的控制权
	dinput_keyboard->Poll();//设置轮询
	//创建鼠标设备
	pancy_dinput->CreateDevice(GUID_SysMouse, &dinput_mouse, NULL);
	dinput_mouse->SetDataFormat(&c_dfDIMouse);//设置设备的数据格式
	dinput_mouse->SetCooperativeLevel(hwnd, mouseCoopFlags);//设置设备的独占等级
	dinput_mouse->Acquire();//获取设备的控制权
	dinput_mouse->Poll();//设置轮询
	return g_Succeed;
}
void PancyInput::GetInput()
{
	//获取鼠标消息
	ZeroMemory(&mouse_buffer, sizeof(mouse_buffer));
	while (true)
	{
		dinput_mouse->Poll();
		dinput_mouse->Acquire();
		HRESULT hr;
		if (SUCCEEDED(dinput_mouse->GetDeviceState(sizeof(mouse_buffer), (LPVOID)&mouse_buffer)))
		{
			break;
		}
		else
		{
			hr = dinput_mouse->GetDeviceState(sizeof(mouse_buffer), (LPVOID)&mouse_buffer);
		}
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED)
		{
			break;
		};
		if (FAILED(dinput_mouse->Acquire()))
		{
			break;
		};
	}
	//获取键盘消息
	ZeroMemory(&key_buffer, sizeof(key_buffer));
	while (true)
	{
		dinput_keyboard->Poll();
		dinput_keyboard->Acquire();
		HRESULT hr;
		if (SUCCEEDED(dinput_keyboard->GetDeviceState(sizeof(key_buffer), (LPVOID)&key_buffer)))
		{
			break;
		}
		else
		{
			hr = dinput_keyboard->GetDeviceState(sizeof(key_buffer), (LPVOID)&key_buffer);
		}
		if (hr != DIERR_INPUTLOST || hr != DIERR_NOTACQUIRED)
		{
			break;
		};
		if (FAILED(dinput_keyboard->Acquire()))
		{
			break;
		};
	}

}
bool PancyInput::CheckKeyboard(int key_value)
{
	if (key_buffer[key_value] & 0x80)
	{
		return true;
	}
	return false;
}
bool PancyInput::CheckMouseDown(int mouse_value)
{
	if ((mouse_buffer.rgbButtons[mouse_value] & 0x80) != 0)
	{
		return true;
	}
	return false;
}
float PancyInput::MouseMove_X()
{
	return (float)mouse_buffer.lX;
}
float PancyInput::MouseMove_Y()
{
	return (float)mouse_buffer.lY;
}
float PancyInput::MouseMove_Z()
{
	return (float)mouse_buffer.lZ;
}