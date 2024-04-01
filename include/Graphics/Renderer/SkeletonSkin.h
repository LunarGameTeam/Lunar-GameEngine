#pragma once
#include "RenderData.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Renderer/RenderMesh.h"


namespace luna::graphics
{

class SubMeshSkeletal;
struct MeshSkeletonLinkClusterBase
{
	LUnorderedMap<int32_t, int32_t> mSkinBoneIndex2SkeletonBoneIndex;
	LArray<LMatrix4f>               mBindposeMatrix;
};

struct AnimationInstanceMatrix
{
	LArray<LMatrix4f> mBoneMatrix;
};

class ShaderParamSceneBuffer
{
	//存储全部材质参数信息的buffer
	RHIResourcePtr shaderParamBuffer;
	//存储全部材质贴图信息的view
	LArray<RHIViewPtr> shaderParamTexture;
	//每个材质参数的大小
	size_t perElementSize;

};

class RENDER_API SkeletonSkinData : public RenderData
{
	LString mSkeletonUniqueName;
public:
	SkeletonSkinData();

	LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;

	void Create(const LString &skeletonUniqueName,SubMeshSkeletal* meshSkeletalData, const LUnorderedMap<LString, int32_t>& skeletonId);

	MeshSkeletonLinkClusterBase* GetMeshSkeletonLinkClusterData(int32_t clusterId);

	void UpdateAnimationInstanceMatrixData(const LArray<LMatrix4f>& allBoneMatrix);

	AnimationInstanceMatrix* GetAnimationInstanceMatrixData();

	RHIView* GetSkinMatrixBuffer() { return mSkeletonResultBufferView.get(); }

public:
	void PerObjectUpdate(RenderObject* renderObject) override;

private:

	MeshSkeletonLinkClusterBase mMeshSkeletonLinkClusterBuffer;

	AnimationInstanceMatrix mAnimationInstanceMatrixBuffer;

	RHIResourcePtr mSkeletonResultBuffer;

	RHIViewPtr     mSkeletonResultBufferView;
};

struct SkeletonBindPoseMatrix
{
	LUnorderedMap<LString, int32_t> mSkeletonId;
};

void RENDER_API MeshRendererUpdateSkinCommand(
	graphics::RenderScene* curScene,
	RenderMeshBridgeData& renderData,
	const LArray<SubMesh*>& initSubmesh,
	const LString& skeletonUniqueName,
	const LArray<SkeletonBindPoseMatrix>& bindCluster
);
//class SkeletonSkinData : public RenderData
//{
//public:
//	SkeletonSkinData();
//
//	LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;
//
//	int32_t AddMeshSkeletonLinkClusterData(SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName);
//
//	int32_t GetMeshSkeletonLinkClusterDataId(SubMesh* meshData, const LString& skeletonUniqueName);
//
//	MeshSkeletonLinkClusterBase* GetMeshSkeletonLinkClusterData(int32_t clusterId);
//
//	int32_t AddAnimationInstanceMatrixData(const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix);
//
//	void UpdateAnimationInstanceMatrixData(int32_t animInstanceId, const LArray<LMatrix4f>& allBoneMatrix);
//
//	int32_t GetAnimationInstanceMatrixDataId(const LString& animaInstanceUniqueName);
//
//	AnimationInstanceMatrix* GetAnimationInstanceMatrixData(int32_t animInstanceId);
//
//	RHIView* GetSkinMatrixBuffer() { return mSkeletonResultBufferView.get(); }
//
//public:
//	void PerSceneUpdate(RenderScene* renderScene) override;
//
//private:
//
//	MemberDataPack<MeshSkeletonLinkClusterBase> mMeshSkeletonLinkClusterBuffer;
//
//	MemberDataPack<AnimationInstanceMatrix> mAnimationInstanceMatrixBuffer;
//
//	LString GetSubmeshName(SubMesh* meshData);
//
//	LString GetClusterName(SubMesh* meshData, const LString& skeletonUniqueName);
//
//	RHIResourcePtr mSkeletonResultBuffer;
//	RHIViewPtr     mSkeletonResultBufferView;
//};


}
