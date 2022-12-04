#include "engine/camera.h"
#include "engine/scene.h"
#include "render/render_module.h"
#include "core/object/entity.h"

namespace luna
{

RegisterTypeEmbedd_Imp(CameraComponent)
{
	cls->Binding<Self>();
	LBindingModule::Get("luna")->AddType(cls);
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

const luna::LFrustum CameraComponent::GetFrustum() const
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

const LVector3f& CameraComponent::GetPosition()const
{
	return mTransform->GetPosition();
}

const float CameraComponent::GetFar()const
{
	return mFar;
}

const float CameraComponent::GetNear()const
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

void CameraComponent::SetAspectRatio(float val)
{
	mAspect = val;
}

void CameraComponent::OnCreate()
{
	mNeedTick = true;
	mTransform = GetEntity()->RequireComponent<Transform>();
	mRenderView = GetScene()->GetRenderScene()->CreateRenderView();
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

const luna::LMatrix4f CameraComponent::GetWorldMatrix()const
{
	return mTransform->GetLocalToWorldMatrix();
}

}