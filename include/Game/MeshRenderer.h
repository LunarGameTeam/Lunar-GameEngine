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


namespace luna::graphics
{

struct GameRenderBridgeDataStaticMesh :public graphics::GameRenderBridgeData
{
	//资源文件在render使用的时候一定保证是加载好的状态了，因此可以放在这里
	bool mNeedIniteMesh = false;
	LArray<SubMesh*> mInitSubmesh;
	bool mNeedIniteMaterial = false;
	LArray<size_t> mMaterialInitIndex;           //哪些材质需要重载
	LArray<MaterialTemplateAsset*> mInitMaterial;//需要重载的材质数据

	//这些是需要实时更新的数据
	bool              NeedUpdateStaticMessage = false;
	bool              mCastShadow = true;
	bool              mReceiveLight = true;
	bool              mReceiveShadow = true;
};

class GameStaticMeshRenderDataUpdater :public graphics::GameRenderDataUpdater
{
protected:
	LArray<graphics::RenderObject*> mRenderObjects;
	LArray<RenderMeshBase*> mRenderMesh;
public:
	virtual ~GameStaticMeshRenderDataUpdater() {};
protected:
	virtual void UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene) override;

	virtual void ClearData(graphics::GameRenderBridgeData* curData) override;

	virtual LSharedPtr<graphics::GameRenderBridgeData> GenarateData() override { return MakeShared<GameRenderBridgeDataStaticMesh>(); };
};


struct SkeletonBindPoseMatrix
{
	LUnorderedMap<LString, int32_t> mSkeletonId;
};

struct GameRenderBridgeDataSkeletalMesh :public GameRenderBridgeDataStaticMesh
{
	bool                            mNeedInitSkinMessage = false;
	LString                         mSkeletonUniqueName;
	LArray<SkeletonBindPoseMatrix>  mBindCluster;
};

class GameSkeletalMeshRenderDataUpdater :public graphics::GameStaticMeshRenderDataUpdater
{
	LArray<SkeletonSkinData*> mSkinData;
public:
	~GameSkeletalMeshRenderDataUpdater() {};
private:
	void UpdateRenderThreadImpl(graphics::GameRenderBridgeData* curData, graphics::RenderScene* curScene) override;

	void ClearData(graphics::GameRenderBridgeData* curData) override;

	virtual LSharedPtr<graphics::GameRenderBridgeData> GenarateData() override { return MakeShared<GameRenderBridgeDataSkeletalMesh>(); };
};

class GAME_API StaticMeshRenderer : public RendererComponent
{
	RegisterTypeEmbedd(StaticMeshRenderer, RendererComponent)
public:
	StaticMeshRenderer() :
		mMaterialAsset(this)
	{

	}

	virtual ~StaticMeshRenderer();

	void SetMeshAsset(MeshAsset* obj);

	void SetMaterialAsset(int32_t matIndex, const LString& assetPath);

	void SetCastShadow(bool val);

	void OnCreate() override;

	void OnActivate() override;

	MeshAsset* GetMeshAsset() { return mMeshAsset.get(); };

protected:
	bool                              mRenderParamDirty = false;

	bool                              mCastShadow = true;

	bool                              mMeshDirty = false;

	bool                              mMaterialDirty = true;

	LArray<size_t>                    mAllDirtyMaterial;

	SharedPtr<MeshAsset>              mMeshAsset;

	TPPtrArray<MaterialTemplateAsset> mMaterialAsset;

	//render相关
	virtual LSharedPtr<graphics::GameRenderDataUpdater> GenarateRenderUpdater() override { return MakeShared<GameStaticMeshRenderDataUpdater>(); }

	virtual LSharedPtr<graphics::GameRenderDataUpdater> OnTickImpl(graphics::GameRenderBridgeData* curRenderData) override;
};

//class GAME_API StaticMeshRenderer2 : public MeshRenderer
//{
//	RegisterTypeEmbedd(StaticMeshRenderer,MeshRenderer)
//public:
//	void SetMeshAsset(MeshAsset* obj);
//	MeshAsset* GetMeshAsset() override { return mMeshAsset.get(); }
//private:
//	void CreateRenderObject() override;
//	SharedPtr<MeshAsset> mMeshAsset;
//	//render相关
//	LSharedPtr<graphics::GameRenderDataUpdater> GenarateRenderUpdater() override { return MakeShared<GameStaticMeshRenderDataUpdater>(); }
//
//	LSharedPtr<graphics::GameRenderDataUpdater> OnTickImpl(graphics::GameRenderBridgeData* curRenderData);
//};
LSharedPtr<graphics::GameRenderDataUpdater> StaticMeshRenderer::OnTickImpl(graphics::GameRenderBridgeData* curRenderData)
{
	GameRenderBridgeDataStaticMesh* realPointer = static_cast<GameRenderBridgeDataStaticMesh*>(curRenderData);
	if (mMeshDirty && mMeshAsset != nullptr && mMaterialAsset.Size() != 0)
	{
		//模型需要初始化
		realPointer->mNeedIniteMesh = true;
		realPointer->mNeedIniteMaterial = true;
		for (int32_t subIndex = 0; subIndex < mMeshAsset->mSubMesh.size(); ++subIndex)
		{
			realPointer->mInitSubmesh.push_back(mMeshAsset->mSubMesh[subIndex]);
			realPointer->mMaterialInitIndex.push_back(subIndex);
			realPointer->mInitMaterial.push_back(mMaterialAsset[subIndex]);
			
		}
		mMeshDirty = false;
		mMaterialDirty = false;
	}
	if (mMaterialDirty)
	{
		//模型材质被改动
		realPointer->mNeedIniteMaterial = true;
		for (size_t refreshIndex = 0; refreshIndex < mAllDirtyMaterial.size(); ++refreshIndex)
		{
			size_t materialIndex = mAllDirtyMaterial[refreshIndex];
			realPointer->mMaterialInitIndex.push_back(materialIndex);
			realPointer->mInitMaterial.push_back(mMaterialAsset[materialIndex]);
		}
		mMaterialDirty = false;
	}
	if (mRenderParamDirty)
	{
		//模型参数被改动
		realPointer->NeedUpdateStaticMessage = true;
		realPointer->mCastShadow = mCastShadow;
		mRenderParamDirty = false;
	}
}

class GAME_API SkeletonMeshRenderer : public StaticMeshRenderer
{
	RegisterTypeEmbedd(SkeletonMeshRenderer, StaticMeshRenderer)
public:
	void SetSkeletonAsset(animation::SkeletonAsset* obj);

	void SetSkelAnimationAsset(animation::AnimationClipAsset* obj);
private:
	bool mSkinDirty = false;

	SharedPtr<animation::SkeletonAsset> mSkeletonAsset;

	SharedPtr<animation::AnimationClipAsset> mSkelAnimAsset;

	animation::SkeletalAnimInstanceBase* mAnimationInstance;

	void GetSkeletonPoseMatrix(LArray<LMatrix4f>& poseMatrix);
	//render相关
	LSharedPtr<graphics::GameRenderDataUpdater> GenarateRenderUpdater() override { return MakeShared<GameSkeletalMeshRenderDataUpdater>(); }

	LSharedPtr<graphics::GameRenderDataUpdater> OnTickImpl(graphics::GameRenderBridgeData* curRenderData) override;
};

LSharedPtr<graphics::GameRenderDataUpdater> SkeletonMeshRenderer::OnTickImpl(graphics::GameRenderBridgeData* curRenderData)
{
	StaticMeshRenderer::OnTickImpl(curRenderData);
	GameRenderBridgeDataSkeletalMesh* realPointer = static_cast<GameRenderBridgeDataSkeletalMesh*>(curRenderData);
	if (mSkinDirty)
	{
		LString skeletonUniqueName = mSkeletonAsset->GetAssetPath();
		realPointer->mSkeletonUniqueName = skeletonUniqueName;
		LArray<LMatrix4f> allBoneMatrix;
		for (int32_t subIndex = 0; subIndex < mMeshAsset->mSubMesh.size(); ++subIndex)
		{
			realPointer->mBindCluster[subIndex].mSkeletonId = mSkeletonAsset->GetSearchIndex();
		}
		mSkinDirty = false;
	}

}

}
