#pragma once

#include "Core/CoreMin.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/MaterialInstance.h"

#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Animation/Asset/SkeletonAsset.h"
#include "Animation/Asset/AnimationClipAsset.h"
#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Game/GameConfig.h"
#include "Core/Object/Component.h"
#include "Graphics/Renderer/RenderMesh.h"


namespace luna::graphics
{

class GAME_API StaticMeshRenderer : public Component
{
	RegisterTypeEmbedd(StaticMeshRenderer, Component)
public:
	StaticMeshRenderer()
	{

	}

	virtual ~StaticMeshRenderer();

	void SetMeshAsset(LString meshAssetPath);

	void SetMaterialAsset(int32_t matIndex, const LString& assetPath);

	void SetCastShadow(bool val);

	void OnCreate() override;

	void OnActivate() override;

	MeshAsset* GetMeshAsset() { return mMeshAsset.get(); };

protected:
	bool                              mRenderParamDirty = false;

	ActionHandle                      mTransformDirtyAction;

	bool                              mTransformDirty = true;

	bool                              mCastShadow = true;

	LArray<size_t>                    mAllDirtyMaterial;

	LString                           mMeshAssetPath;

	SharedPtr<MeshAsset>              mMeshAsset;

	SharedPtr<MaterialTemplateAsset>  mMaterialAsset = nullptr;

	graphics::RenderMeshBridgeData    mVirtualRenderData;

	void                              OnTransformDirty(Transform* transform);

	virtual void                      OnMeshAssetChange();

	virtual void                      GenerateMeshAsset(const LString& path);
};

class GAME_API SkeletonMeshRenderer : public StaticMeshRenderer
{
	RegisterTypeEmbedd(SkeletonMeshRenderer, StaticMeshRenderer)
public:
	void SetSkeletonAsset(animation::SkeletonAsset* obj);

	void SetSkelAnimationAsset(animation::AnimationClipAsset* obj);
private:
	SharedPtr<animation::SkeletonAsset> mSkeletonAsset;

	SharedPtr<animation::AnimationClipAsset> mSkelAnimAsset;

	animation::SkeletalAnimInstanceBase* mAnimationInstance;

	void GetSkeletonPoseMatrix(LArray<LMatrix4f>& poseMatrix);

	void OnMeshAssetChange() override;

	void GenerateMeshAsset(const LString& path) override;

	void UpdateSkinData();
};

}
