#include "editor/ui/scene_panel.h"
#include "editor/editor_module.h"

#include "engine/scene_module.h"

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
	ImVec2 new_size = ImGui::GetContentRegionAvail();

	if (ImGui::IsMouseDragging(0))
	{
		if (new_size.x != mOldSize.x)
		{
			mDragging = true;
		}
	}
	if (mDragging)
	{
		if (ImGui::IsMouseReleased(0))
		{
			mNeedUpdate = true;
			mDragging = false;
		}
	}
	auto scene = sSceneModule->GetScene(0);
	if (mNeedUpdate)
	{
		if (scene)
		{
			render::RenderTarget* rt = sRenderModule->GetRenderScene(0)->GetRenderView(0)->GetRenderTarget();
			rt->SetWidth((uint32_t)new_size.x);
			rt->SetHeight((uint32_t)new_size.y);
		}

		mNeedUpdate = false;

		if (scene)
		{
			const TPPtrArray<Entity>& entities = scene->GetAllEntities();
			for (auto it = entities.begin(); it != entities.end(); ++it)
			{
				CameraComponent* cameraComp = it->Get()->GetComponent<CameraComponent>();
				if (cameraComp != nullptr)
				{
					cameraComp->SetAspectRatio(new_size.x / new_size.y);
				}
			}
		}
	}
// 	if (mImguiTexture)
// 		ImGui::Image((ImTextureID)(mImguiTexture->mImg), ImGui::GetContentRegionAvail());
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;
		if (ImGui::IsMouseHoveringRect(vMin, vMax) && mCamera)
		{
			ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

			auto* transform = mCamera->GetTransform();
			float delta_y = delta.y / 500.0f;
			float delta_x = delta.x / 500.0f;
			auto v = Eigen::AngleAxisf(delta_y * (float)std::numbers::pi, LVector3f::UnitX());
			auto h = Eigen::AngleAxisf(delta_x * (float)std::numbers::pi, LVector3f::UnitY());
			auto pos = transform->GetPosition();
			auto rotation = transform->GetRotation();
			rotation = h * rotation  * v;
			transform->SetRotation(rotation);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
		}
	}


	mOldSize = new_size;
	mOldSize = new_size;
}

void ScenePanel::Init()
{
	auto func = std::bind(&ScenePanel::OnInputEvent, this, std::placeholders::_1, std::placeholders::_2);
	static auto handle = sEventModule->OnInput.Bind(func);
	mImguiTexture = sRenderModule->AddImguiTexture("SceneColor", sRenderModule->mMainRT->mColorTexture);
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
