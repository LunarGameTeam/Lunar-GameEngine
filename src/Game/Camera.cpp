#include "Game/Camera.h"
#include "Game/Scene.h"
#include "Graphics/RenderModule.h"
#include "Core/Object/Entity.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"

namespace luna
{


RegisterTypeEmbedd_Imp(CameraSystem)
{
	cls->Ctor<CameraSystem>();
	BindingModule::Get("luna")->AddType(cls);	
	RegisterSystem(cls);
}

CameraSystem::CameraSystem()
{
	FocusComponentType<CameraComponent>();
	RequireComponentType<Transform>();
}

void CameraSystem::OnTick(float deltaTime)
{
	auto& transforms = GetRequireComponents<Transform>();
	Foreach<CameraComponent>([&transforms](uint32_t idx, CameraComponent* camera)
	{
		Transform* transform = (Transform *)(transforms[idx]);
	});
}

RegisterTypeEmbedd_Imp(CameraComponent)
{
	cls->Binding<Self>();
	cls->BindingProperty<&CameraComponent::mAspect>("aspect");

	cls->BindingMethod<&CameraComponent::ViewportToWorldPosition>("viewport_to_worldpos");
	cls->BindingMethod<&CameraComponent::WorldPositionToViewport>("worldpos_to_viewport");

	cls->VirtualProperty("render_target")
		.Getter<&CameraComponent::GetRenderViewTarget>()
		.Setter<&CameraComponent::SetRenderViewTarget>()
		.Binding<CameraComponent, render::RenderTarget*>();
	cls->VirtualProperty("transform")
		.Getter<&CameraComponent::GetTransform>()
		.Binding<CameraComponent, Transform*>();

	cls->BindingProperty<&CameraComponent::mViewMat>("view_matrix");
	cls->BindingProperty<&CameraComponent::mProjMat>("proj_matrix");


	cls->BindingProperty<&CameraComponent::mSpeed>("fly_speed");
	cls->BindingProperty<&CameraComponent::mDirection>("direction");


	BindingModule::Get("luna")->AddType(cls);
	cls->Ctor<CameraComponent>();
}


const LMatrix4f &CameraComponent::GetViewMatrix()const
{
	mViewMat = mTransform->GetLocalToWorldMatrix().inverse();
	return mViewMat;
}

const LMatrix4f &CameraComponent::GetProjectionMatrix()const
{
	LMath::GenPerspectiveFovLHMatrix(mProjMat, mFovY, mAspect, mNear, mFar);
	return mProjMat;
}

const LFrustum CameraComponent::GetFrustum() const
{
	float half_fov = mFovY * 0.5f;
	float half_y = std::tan(half_fov) * mNear;
	float half_x = half_y * mAspect;
	LFrustum f;
	f.near_pos[0] = LVector4f(-half_x, half_y, mNear, 1);	
	f.near_pos[1] = LVector4f(half_x, half_y, mNear, 1);
	f.near_pos[2] = LVector4f(half_x, -half_y, mNear, 1);
	f.near_pos[3] = LVector4f(-half_x, -half_y, mNear, 1);

	float half_far_y = std::tan(half_fov) * mFar;
	float half_far_x = half_far_y * mAspect;

	f.far_pos[0] = LVector4f(-half_far_x, half_far_y, mFar, 1);
	f.far_pos[1] = LVector4f(half_far_x, half_far_y, mFar, 1);
	f.far_pos[2] = LVector4f(half_far_x, -half_far_y, mFar, 1);
	f.far_pos[3] = LVector4f(-half_far_x, -half_far_y, mFar, 1);


	return f.Multiple(mTransform->GetLocalToWorldMatrix());

}

LVector3f CameraComponent::GetPosition() const
{
	return mTransform->GetPosition();
}

float CameraComponent::GetFar() const
{
	return mFar;
}

float CameraComponent::GetNear()const
{
	return mNear;
}

void CameraComponent::SetFar(float val)
{
	mFar = val;
}

void CameraComponent::SetNear(float val)
{
	mNear = val;
}

render::RenderTarget* CameraComponent::GetRenderViewTarget()
{
	if (mRenderView)
	{
		if(mRenderView->GetRenderTarget())
			return mRenderView->GetRenderTarget();
		return sRenderModule->mMainRT.Get();
	}
	return nullptr;
}

void CameraComponent::SetRenderViewTarget(render::RenderTarget* target)
{
	mRenderView->SetRenderTarget(target);
}

void CameraComponent::SetAspectRatio(float val)
{
	mAspect = val;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::OnCreate()
{
	mNeedTick = true;
	mTransform = GetEntity()->RequireComponent<Transform>();
}

void CameraComponent::OnActivate()
{
	if (mRenderView == nullptr)
	{
		mRenderView = GetScene()->GetRenderScene()->CreateRenderView();
	}	
}

void CameraComponent::OnDeactivate()
{
	if (GetScene() && GetScene()->GetRenderScene())
	{
		GetScene()->GetRenderScene()->DestroyRenderView(mRenderView);
		mRenderView = nullptr;
	}

}

void CameraComponent::OnTick(float delta_time)
{
	auto pos = mTransform->GetPosition();
	if (mSpeed > 0.01)
	{
		
		pos = pos + mDirection * mSpeed * delta_time;
		mTransform->SetPosition(pos);
	}
	mRenderView->SetViewPosition(pos);
	mRenderView->SetViewMatrix(GetViewMatrix());
	mRenderView->SetProjectionMatrix(GetProjectionMatrix());
}

LVector3f CameraComponent::ViewportToWorldPosition(const LVector2f& viewport)
{
	return LVector3f(0, 0, 0);
}

LVector2f CameraComponent::WorldPositionToViewport(const LVector3f& worldpos)
{
	return LVector2f(0, 0);
}

const luna::LMatrix4f CameraComponent::GetWorldMatrix()const
{
	return mTransform->GetLocalToWorldMatrix();
}

}