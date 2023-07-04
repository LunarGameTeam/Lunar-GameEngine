#pragma once

#include "Core/CoreMin.h"

#include "Game/RenderComponent.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Renderer/MaterialInstance.h"

#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Animation/Asset/SkeletonAsset.h"
#include "Animation/Asset/AnimationClipAsset.h"
#include "Animation/SkeletonAnimation/SkeletalAnimInstanceBase.h"

#include "Graphics/Asset/MaterialTemplate.h"


namespace luna::render
{

class GAME_API MeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(MeshRenderer, RendererComponent)
public:
	MeshRenderer() :
		mMaterialInstance(this)
	{

	}

	virtual ~MeshRenderer();

	void SetMaterial(MaterialTemplateAsset* mat);
	void SetCastShadow(bool val);
	void OnCreate() override;

	virtual MeshAsset* GetMeshAsset() { return nullptr; };

	void OnActivate() override;
protected:
	virtual void CreateRenderObject() {};
protected:
	bool mCastShadow = true;
	uint64_t mRO = uint64_t(-1);
	SharedPtr<render::MaterialTemplateAsset> mMaterialAsset;
	TPPtr<render::MaterialInstance> mMaterialInstance;

};

class GAME_API StaticMeshRenderer : public MeshRenderer
{
	RegisterTypeEmbedd(StaticMeshRenderer,MeshRenderer)
public:
	void SetMeshAsset(MeshAsset* obj);
	MeshAsset* GetMeshAsset() override { return mMeshAsset.get(); }
private:
	void CreateRenderObject() override;
	SharedPtr<MeshAsset> mMeshAsset;
};


class GAME_API SkeletonMeshRenderer : public MeshRenderer
{
	RegisterTypeEmbedd(SkeletonMeshRenderer, MeshRenderer)
public:
	MeshAsset* GetMeshAsset() override { return mSkeletalMeshAsset.get(); }
	
	void SetMeshAsset(SkeletalMeshAsset* obj);
	
	void SetSkeletonAsset(animation::SkeletonAsset* obj);

	void SetSkelAnimationAsset(animation::AnimationClipAsset* obj);
private:
	
	SharedPtr<animation::SkeletonAsset> mSkeletonAsset;

	SharedPtr<animation::AnimationClipAsset> mSkelAnimAsset;
	
	SharedPtr<SkeletalMeshAsset> mSkeletalMeshAsset;

	animation::SkeletalAnimInstanceBase* mAnimationInstance;

	void CreateRenderObject() override;

	void GetSkeletonPoseMatrix(LArray<LMatrix4f>& poseMatrix);

	void UpdateAnimationInstanceRo();
};

}
