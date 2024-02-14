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
}

void CameraSystem::OnTick(float deltaTime)
{
	Foreach<CameraComponent>([](uint32_t idx, CameraComponent* camera, Entity* entity)
	{
		Transform* transform = entity->GetComponent<Transform>();
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
		.Binding<CameraComponent, graphics::RenderTarget*>();
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
	LFrustum f = LFrustum::MakeFrustrum(mFovY, mNear, mFar, mAspect);
	f.Multiple(mTransform->GetLocalToWorldMatrix());
	return f;
}
CameraComponent::CameraComponent():mTarget(this)
{
	mTarget.SetPtr(NewObject<graphics::RenderTarget>());
	mTarget->Ready();
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
	graphics::RenderViewDataFarPlaneUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mFar);
}

void CameraComponent::SetNear(float val)
{
	mNear = val;
	graphics::RenderViewDataNearPlaneUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mNear);
}

graphics::RenderTarget* CameraComponent::GetRenderViewTarget()
{
	return mTarget.Get();
}

void CameraComponent::SetRenderViewTarget(graphics::RenderTarget* target)
{
	mTarget = target;
	graphics::RenderViewDataRenderTargetUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mTarget.Get());
}

void CameraComponent::SetAspectRatio(float val)
{
	mAspect = val;
}

CameraComponent::~CameraComponent()
{
}
void CameraComponent::OnTransformDirty(Transform* transform)
{
	graphics::RenderViewDataPositionUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mTransform->GetPosition());
	graphics::RenderViewDataViewMatrixUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, GetViewMatrix());
}
void CameraComponent::OnRenderTargetDirty(graphics::RenderTarget* renderTarget)
{
	mTarget = renderTarget;
	graphics::RenderViewDataRenderTargetUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mTarget.Get());
	graphics::RenderViewDataTargetWidthUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mTarget->GetWidth());
	graphics::RenderViewDataTargetHeightUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mTarget->GetHeight());
}

void CameraComponent::OnCreate()
{
	Super::OnCreate();
	Transform* newTransform = GetEntity()->RequireComponent<Transform>();
	mTransformDirtyAction = newTransform->OnTransformDirty.Bind(AutoBind(&CameraComponent::OnTransformDirty, this));
	mRenderTargetDirtyAction = mTarget->OnRenderTargetDirty.Bind(AutoBind(&CameraComponent::OnRenderTargetDirty, this));
	OnTransformDirty(newTransform);
	graphics::RenderViewDataGenerateCommand(GetScene()->GetRenderScene(), mVirtualRenderData);
	graphics::RenderViewDataFarPlaneUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mFar);
	graphics::RenderViewDataNearPlaneUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mNear);
	graphics::RenderViewDataFovUpdateCommand(GetScene()->GetRenderScene(), mVirtualRenderData, mFovY);
}

void CameraComponent::OnActivate()
{
}

void CameraComponent::OnDeactivate()
{

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