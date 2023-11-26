#include "Game/Camera.h"
#include "Game/Scene.h"
#include "Graphics/RenderModule.h"
#include "Core/Object/Entity.h"
#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"

namespace luna
{

void GameCameraRenderDataUpdater::UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene)
{
	GameRenderBridgeDataCamera* realPointer = static_cast<GameRenderBridgeDataCamera*>(curData);

	if (realPointer->mIntrinsicsDirty)
	{
		if (mRenderView == nullptr)
		{
			mRenderView = curScene->CreateRenderView();
			mRenderView->mViewType = graphics::RenderViewType::SceneView;
			graphics::RenderViewParameterData* viewParamData = curScene->RequireData<graphics::RenderViewParameterData>();
			viewParamData->Init();
			mViewCbuffer = MakeShared<graphics::ShaderCBuffer>(viewParamData->GetParamDesc());
		}
		float curAspect = (float)realPointer->mIntrinsicsParameter.mRtWidth / (float)realPointer->mIntrinsicsParameter.mRtHeight;
		LMath::GenPerspectiveFovLHMatrix(mProjMatrix, realPointer->mIntrinsicsParameter.mFovY, curAspect, realPointer->mIntrinsicsParameter.mNear, realPointer->mIntrinsicsParameter.mFar);
	}

	if (realPointer->mIntrinsicsDirty || realPointer->mExtrinsicsDirty)
	{
		LVector2f cNearFar(realPointer->mIntrinsicsParameter.mNear, realPointer->mIntrinsicsParameter.mFar);
		mViewCbuffer->Set("cNearFar", cNearFar);
		mViewCbuffer->Set("cProjectionMatrix", mProjMatrix);
		mViewCbuffer->Set("cViewMatrix", realPointer->mExtrinsicsParameter.mViewMatrix);
		mViewCbuffer->Set("cCamPos", realPointer->mExtrinsicsParameter.mPosition);

		graphics::RenderViewParameterData* viewParamData = curScene->GetData<graphics::RenderViewParameterData>();
		graphics::RHIResource* viewBufferStage = curScene->GetStageBufferPool()->AllocUniformStageBuffer(mViewCbuffer.get());
		graphics::GpuSceneUploadCopyCommand* copyCommand = curScene->AddCopyCommand();
		copyCommand->mSrcOffset = 0;
		copyCommand->mDstOffset = 0;
		copyCommand->mCopyLength = SizeAligned2Pow(mViewCbuffer->mData.size(),256);
		copyCommand->mUniformBufferInput = viewBufferStage;
		copyCommand->mStorageBufferOutput = viewParamData->GetResource();
	}
}

GameCameraRenderDataUpdater::~GameCameraRenderDataUpdater()
{
	if (mRenderView)
	{
		mRenderView->mOwnerScene->DestroyRenderView(mRenderView);
	}
};

void GameCameraRenderDataUpdater::ClearData(graphics::GameRenderBridgeData* curData)
{
	GameRenderBridgeDataCamera* realPointer = static_cast<GameRenderBridgeDataCamera*>(curData);
	realPointer->mIntrinsicsDirty = false;
	realPointer->mExtrinsicsDirty = false;
}

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
	mNeedUpdateIntrinsics = true;
	mFar = val;
}

void CameraComponent::SetNear(float val)
{
	mNeedUpdateIntrinsics = true;
	mNear = val;
}

graphics::RenderTarget* CameraComponent::GetRenderViewTarget()
{
	//if (mRenderView)
	//{
	//	if(mRenderView->GetRenderTarget())
	//		return mRenderView->GetRenderTarget();
	//	return sRenderModule->mMainRT.Get();
	//}
	return nullptr;
}

void CameraComponent::SetRenderViewTarget(graphics::RenderTarget* target)
{
	mTarget = target;
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
	mNeedUpdateExtrinsics = true;

}
void CameraComponent::OnCreate()
{
	mNeedUpdateIntrinsics = true;
	mNeedUpdateExtrinsics = true;
	Transform* newTransform = GetEntity()->RequireComponent<Transform>();
	mTransformDirtyAction = newTransform->OnTransformDirty.Bind(AutoBind(&CameraComponent::OnTransformDirty, this));
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

LSharedPtr<graphics::GameRenderDataUpdater> CameraComponent::OnTickImpl(graphics::GameRenderBridgeData* curRenderData)
{
	GameRenderBridgeDataCamera* realPointer = static_cast<GameRenderBridgeDataCamera*>(curRenderData);

	if (mNeedUpdateIntrinsics)
	{
		realPointer->mIntrinsicsDirty = true;
		realPointer->mIntrinsicsParameter.mFar = mFar;
		realPointer->mIntrinsicsParameter.mFovY = mFovY;
		realPointer->mIntrinsicsParameter.mNear = mNear;
		realPointer->mIntrinsicsParameter.mRtHeight = mTarget->GetHeight();
		realPointer->mIntrinsicsParameter.mRtWidth = mTarget->GetWidth();
	}

	if (mNeedUpdateIntrinsics)
	{
		realPointer->mExtrinsicsDirty = true;
		auto pos = mTransform->GetPosition();
		realPointer->mExtrinsicsParameter.mPosition = pos;
		realPointer->mExtrinsicsParameter.mViewMatrix = GetViewMatrix();
	}
}

}