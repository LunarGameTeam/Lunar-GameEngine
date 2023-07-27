#pragma once
#include "RenderData.h"
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Graphics/Renderer/MaterialInstance.h"


namespace luna::graphics
{

struct MeshSkeletonLinkClusterBase
{
	LUnorderedMap<int32_t, int32_t> mSkinBoneIndex2SkeletonBoneIndex;
	LArray<LMatrix4f>               mBindposeMatrix;
};

struct AnimationInstanceMatrix
{
	LArray<LMatrix4f> mBoneMatrix;
};

template<typename DataType>
class MemberDataPack
{
	LQueue<size_t>                               mEmptyId;
	LUnorderedMap<LString, int32_t>              mDataName;
	LUnorderedMap<int32_t, LSharedPtr<DataType>> mDataBuffer;
public:
	int32_t CheckDataIdByName(const LString& uniqueDataName)
	{
		auto itor = mDataName.find(uniqueDataName);
		if (itor == mDataName.end())
		{
			return -1;
		}
		return itor->second;
	};

	int32_t AddData(const LString& uniqueDataName)
	{
		int32_t dataIndex = CheckDataIdByName(uniqueDataName);
		if (dataIndex != -1)
		{
			return dataIndex;
		}
		if (mEmptyId.empty())
		{
			dataIndex = mDataName.size();
		}
		else
		{
			dataIndex = mEmptyId.front();
			mEmptyId.pop();
		}
		mDataName.insert({ uniqueDataName ,dataIndex });
		mDataBuffer[dataIndex] = MakeShared<DataType>();
		return dataIndex;
	}

	DataType* GetData(int32_t meshId)
	{
		auto curData = mDataBuffer.find(meshId);
		if (curData == mDataBuffer.end())
		{
			return nullptr;
		}
		return curData->second.get();
	}
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

class SkeletonSkinData : public RenderData
{
public:
	SkeletonSkinData();

	LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;

	int32_t AddMeshSkeletonLinkClusterData(SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName);

	int32_t GetMeshSkeletonLinkClusterDataId(SubMesh* meshData, const LString& skeletonUniqueName);

	MeshSkeletonLinkClusterBase* GetMeshSkeletonLinkClusterData(int32_t clusterId);

	int32_t AddAnimationInstanceMatrixData(const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix);

	void UpdateAnimationInstanceMatrixData(int32_t animInstanceId, const LArray<LMatrix4f>& allBoneMatrix);

	int32_t GetAnimationInstanceMatrixDataId(const LString& animaInstanceUniqueName);

	AnimationInstanceMatrix* GetAnimationInstanceMatrixData(int32_t animInstanceId);

	RHIView* GetSkinMatrixBuffer() { return mSkeletonResultBufferView.get(); }

public:
	void PerSceneUpdate(RenderScene* renderScene) override;

private:

	MemberDataPack<MeshSkeletonLinkClusterBase> mMeshSkeletonLinkClusterBuffer;

	MemberDataPack<AnimationInstanceMatrix> mAnimationInstanceMatrixBuffer;

	LString GetSubmeshName(SubMesh* meshData);

	LString GetClusterName(SubMesh* meshData, const LString& skeletonUniqueName);

	RHIResourcePtr mSkeletonResultBuffer;
	RHIViewPtr     mSkeletonResultBufferView;
};


}
