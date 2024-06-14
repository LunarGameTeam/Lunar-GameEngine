#include "Core/Platform/PlatformModule.h"


#include "Core/CoreMin.h"
#include "Core/Platform/Window.h"
#include "Core/Framework/LunaCore.h"
#include "Core/Event/EventModule.h"
#include "Core/Asset/AssetModule.h"
#include "Core/Asset/Asset.h"

#include "imgui_impl_sdl2.h"


LUnorderedMap<Sint32, luna::KeyCode> s_key_map;
namespace luna
{

CONFIG_IMPLEMENT(int, Window, UsingImGUI, 1);
CONFIG_IMPLEMENT(int, Window, DefaultWidth, 1024);
CONFIG_IMPLEMENT(int, Window, DefaultHeight, 768);

CONFIG_IMPLEMENT(LString, Start, DefaultProject, "");

RegisterTypeEmbedd_Imp(PlatformModule)
{
	cls->Ctor<PlatformModule>();
	cls->Binding<PlatformModule>();
	cls->BindingMethod<&Self::SetProjectDir>("set_project_dir");
	cls->VirtualProperty("main_window")
		.Getter<&PlatformModule::GetMainWindow>()
		.Binding<PlatformModule, LWindow*>();

	cls->VirtualProperty("project_dir")
		.Getter<&Self::GetProjectDir>()
		.Binding<PlatformModule, LString>();
	cls->VirtualProperty("engine_dir")
		.Getter<&Self::GetEngineDir>()
		.Binding<PlatformModule, LString>();
	BindingModule::Luna()->AddType(cls);
}


CORE_API PlatformModule* sPlatformModule = nullptr;

PlatformModule::PlatformModule()
{
	assert(sPlatformModule == nullptr);
	sPlatformModule = this;
	mNeedTick = true;
}

bool PlatformModule::OnLoad()
{
#ifdef _WIN64
	mPlatformFile = new WindowsFileManager();
	mPlatformFile->InitFileManager();
#endif
	mPlatformFile->SetProjectDir(Config_DefaultProject.GetValue());
	return true;
}

LWindow* PlatformModule::CreateLunaWindow(const luna::LString& name, int width, int height)
{
#ifdef _WIN64
	auto win32Window = new LWindow(width, height);
	win32Window->Init();
	mWindows[win32Window->Id()] = win32Window;
#endif // _WIN32

	return win32Window;
}

LWindow* PlatformModule::GetMainWindow()
{
	return mMainWindow;
}

bool PlatformModule::OnInit()
{

	s_key_map = {
		{ SDL_KeyCode::SDLK_0, KeyCode::Num0},
		{ SDL_KeyCode::SDLK_1, KeyCode::Num1},
		{ SDL_KeyCode::SDLK_2, KeyCode::Num2},
		{ SDL_KeyCode::SDLK_3, KeyCode::Num3},
		{ SDL_KeyCode::SDLK_4, KeyCode::Num4},
		{ SDL_KeyCode::SDLK_5, KeyCode::Num5},
		{ SDL_KeyCode::SDLK_6, KeyCode::Num6},
		{ SDL_KeyCode::SDLK_7, KeyCode::Num7},
		{ SDL_KeyCode::SDLK_8, KeyCode::Num8},
		{ SDL_KeyCode::SDLK_9, KeyCode::Num9},
		{ SDL_KeyCode::SDLK_q, KeyCode::Q},
		{ SDL_KeyCode::SDLK_w, KeyCode::W},
		{ SDL_KeyCode::SDLK_e, KeyCode::E},
		{ SDL_KeyCode::SDLK_r, KeyCode::R},
		{ SDL_KeyCode::SDLK_t, KeyCode::T},
		{ SDL_KeyCode::SDLK_y, KeyCode::Y},
		{ SDL_KeyCode::SDLK_u, KeyCode::U},
		{ SDL_KeyCode::SDLK_i, KeyCode::I},
		{ SDL_KeyCode::SDLK_o, KeyCode::O},
		{ SDL_KeyCode::SDLK_p, KeyCode::P},
		{ SDL_KeyCode::SDLK_a, KeyCode::A},
		{ SDL_KeyCode::SDLK_s, KeyCode::S},
		{ SDL_KeyCode::SDLK_d, KeyCode::D},
		{ SDL_KeyCode::SDLK_f, KeyCode::F},
		{ SDL_KeyCode::SDLK_g, KeyCode::G},
		{ SDL_KeyCode::SDLK_h, KeyCode::H},
		{ SDL_KeyCode::SDLK_j, KeyCode::J},
		{ SDL_KeyCode::SDLK_k, KeyCode::K},
		{ SDL_KeyCode::SDLK_l, KeyCode::L},
		{ SDL_KeyCode::SDLK_z, KeyCode::Z},
		{ SDL_KeyCode::SDLK_x, KeyCode::X},
		{ SDL_KeyCode::SDLK_c, KeyCode::C},
		{ SDL_KeyCode::SDLK_v, KeyCode::V},
		{ SDL_KeyCode::SDLK_b, KeyCode::B},
		{ SDL_KeyCode::SDLK_n, KeyCode::N},
		{ SDL_KeyCode::SDLK_m, KeyCode::M},
		{ SDL_KeyCode::SDLK_ESCAPE, KeyCode::Escape},
		{ SDL_KeyCode::SDLK_SPACE, KeyCode::Space},
		{ SDL_KeyCode::SDLK_UP, KeyCode::Up},
		{ SDL_KeyCode::SDLK_DOWN, KeyCode::Down},
		{ SDL_KeyCode::SDLK_LEFT, KeyCode::Left},
		{ SDL_KeyCode::SDLK_RIGHT, KeyCode::Right},
	};

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	mMainWindow = CreateLunaWindow("MainWindow", Config_DefaultWidth.GetValue(), Config_DefaultHeight.GetValue());


	return true;
}

bool PlatformModule::OnShutdown()
{
	mPlatformFile->DisposeFileManager();
	delete mPlatformFile;
	return true;
}

LWindow* PlatformModule::GetWindowByHandle(WindowHandle handle)
{
	return mWindows[handle];
}

LString PlatformModule::GetEngineDir()
{
	return mPlatformFile->EngineDir();
}

LString PlatformModule::GetProjectDir()
{
	return mPlatformFile->ProjectDir();
}

void PlatformModule::Tick(float delta_time)
{
	LModule::Tick(delta_time);
	static EventModule* event_subsystem = gEngine->GetTModule<EventModule>();
	SDL_Event event;
	bool done = true;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			gEngine->Exit();
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
			gEngine->Exit();
		auto window = mWindows[event.window.windowID];
		switch (event.type)
		{
		case SDL_WINDOWEVENT:
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			{
				WindowEvent window_event;
				int w = event.window.data1;
				int h = event.window.data2;
				window_event.width = w;
				window_event.height = h;
				window_event.type = EventType::Window_Resize;
				//sPlatformModule->OnWindowResize.BroadCast(*window, window_event);
				break;
			}
			default:
				break;
			}
			break;
		}
		case SDL_MOUSEMOTION:
			InputEvent input;

			input.type = EventType::Input_MouseMove;
			input.x = (float)event.button.x;
			input.y = (float)event.button.y;
			event_subsystem->OnInput.BroadCast(*window, input);
			break;
		case SDL_KEYDOWN:
		{

			if (event.key.repeat != 0)
				return;
			InputEvent input;

			input.left_ctrl = event.key.keysym.mod & SDL_Keymod::KMOD_LCTRL;
			input.right_ctrl = event.key.keysym.mod & SDL_Keymod::KMOD_RCTRL;
			input.left_shift = event.key.keysym.mod & SDL_Keymod::KMOD_LSHIFT;
			input.right_shift = event.key.keysym.mod & SDL_Keymod::KMOD_RSHIFT;

			input.type = EventType::Input_KeyDown;
			input.code = s_key_map[(Sint32)event.key.keysym.sym];
			input.x = 0;
			input.y = 0;
			event_subsystem->OnInput.BroadCast(*window, input);
			break;
		}
		case SDL_KEYUP:
		{
			InputEvent input;

			input.left_ctrl = event.key.keysym.mod & SDL_Keymod::KMOD_LCTRL;
			input.right_ctrl = event.key.keysym.mod & SDL_Keymod::KMOD_RCTRL;
			input.left_shift = event.key.keysym.mod & SDL_Keymod::KMOD_LSHIFT;
			input.right_shift = event.key.keysym.mod & SDL_Keymod::KMOD_RSHIFT;

			input.type = EventType::Input_KeyUp;
			input.code = s_key_map[(Sint32)event.key.keysym.sym];
			input.x = 0;
			input.y = 0;
			event_subsystem->OnInput.BroadCast(*window, input);
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			InputEvent input;
			input.type = EventType::Input_MousePress;
			if (event.button.button == SDL_BUTTON_LEFT)
				input.code = KeyCode::MouseLeft;
			if (event.button.button == SDL_BUTTON_RIGHT)
				input.code = KeyCode::MouseRight;
			input.x = (float)event.button.x;
			input.y = (float)event.button.y;
			event_subsystem->OnInput.BroadCast(*window, input);
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			InputEvent input;
			input.type = EventType::Input_MouseRelease;
			if (event.button.button == SDL_BUTTON_LEFT)
				input.code = KeyCode::MouseLeft;
			if (event.button.button == SDL_BUTTON_RIGHT)
				input.code = KeyCode::MouseRight;
			input.x = (float)event.button.x;
			input.y = (float)event.button.y;
			event_subsystem->OnInput.BroadCast(*window, input);
			break;
		}
		default:
			break;
		}
	}
	if (mMainWindow && !gEngine->GetPendingExit())
	{
		if (!mMainWindow->Tick())
			mMainWindow->OnDestroy();
	}
}

}