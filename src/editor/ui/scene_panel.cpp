#include "editor/ui/scene_panel.h"
#include "editor/editor_module.h"

#include "game/scene_module.h"

#include "render/rhi/DirectX12/dx12_descriptor_impl.h"
#include "render/rhi/DirectX12/dx12_view.h"
#include "core/event/event_module.h"


namespace luna::editor
{

RegisterTypeEmbedd_Imp(ScenePanel)
{
	cls->Ctor<ScenePanel>();
	cls->Binding<ScenePanel>();
	BindingModule::Get("luna.editor")->AddType(cls);
};


void ScenePanel::OnGUI()
{

}

void ScenePanel::Init()
{
	auto func = std::bind(&ScenePanel::OnInputEvent, this, std::placeholders::_1, std::placeholders::_2);
	static auto handle = sEventModule->OnInput.Bind(func);	
}

void ScenePanel::OnInputEvent(LWindow& window, InputEvent& event)
{
	return;
	if (!mFocus)
		return;

	auto* transform = mCamera->GetTransform();

	static bool hold = false;
	static float old_x = 0;
	static float old_y = 0;
	static LVector3f delta = LVector3f::Zero();
	auto pos = transform->GetPosition();
	auto rotation = transform->GetRotation();

	switch (event.type)
	{
	case EventType::Input_MousePress:
	{
		old_x = event.x;
		old_y = event.y;
		Log("Editor", "Press");
		hold = true;
		break;
	}
	case EventType::Input_MouseRelease:
	{
		Log("Editor", "Release");
		hold = false;
		break;
	}
	case EventType::Input_MouseMove:
	{
		if (mDragging)
		{
			float x = event.x;
			float y = event.y;
			float delta_x = (x - old_x) / 500;
			float delta_y = (y - old_y) / 500;

		}

		break;
	}
	case EventType::Input_KeyUp:
	{
		if (event.code == KeyCode::W)
		{
			delta.z() -= 1;
		}
		else if (event.code == KeyCode::A)
		{
			delta.x() += 1;
		}
		else if (event.code == KeyCode::S)
		{
			delta.z() += 1;
		}
		else if (event.code == KeyCode::D)
		{
			delta.x() -= 1;
		}
		else if (event.code == KeyCode::Q)
		{
			delta.y() += 1;
		}
		else if (event.code == KeyCode::E)
		{
			delta.y() -= 1;
		}
		if (delta.size() > 0.01)
		{
			LVector3f direction = transform->GetRotation() * delta;
			mCamera->SetFlyDirection(direction);
			mCamera->SetSpeed(10);
		}
		else
			mCamera->SetSpeed(0);
		break;
	}
	case EventType::Input_KeyDown:
	{
		if (event.code == KeyCode::W)
		{
			delta.z() += 1;
		}
		else if (event.code == KeyCode::A)
		{
			delta.x() -= 1;
		}
		else if (event.code == KeyCode::S)
		{
			delta.z() -= 1;
		}
		else if (event.code == KeyCode::D)
		{
			delta.x() += 1;
		}
		else if (event.code == KeyCode::Q)
		{
			delta.y() -= 1;
		}
		else if (event.code == KeyCode::E)
		{
			delta.y() += 1;
		}
		if (delta.size() > 0.01)
		{
			LVector3f direction = transform->GetRotation() * delta;
			mCamera->SetFlyDirection(direction);
			mCamera->SetSpeed(10);
		}
		else
			mCamera->SetSpeed(0);
		break;
	}
	}
}

void ScenePanel::UpdateRT()
{
	mNeedUpdate = true;
}

void ScenePanel::SetScene(Scene* val)
{
	mScene = val;
	for (auto& it : sSceneModule->GetScene(0)->GetAllEntities())
	{
		mCamera = it->GetComponent<CameraComponent>();
		if (mCamera)
			break;
	}
}

}
