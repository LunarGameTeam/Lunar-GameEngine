#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Graphics/Renderer/RenderMesh.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"

#include <functional>

namespace luna::render
{
template<typename DataType>
class AssetSceneMemberDataPack
{
	LQueue<size_t> mEmptyId;
	LUnorderedMap<LString, int32_t> mDataName;
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

	int32_t AddData(const LString &uniqueDataName)
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
class AssetSceneData
{
public:
	AssetSceneData();
	LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;
	//mesh������ֻ��Ӳ�ɾ������ʱ���ٴ���ro�뿪������ôɾ����Ӧ��scene��Դ������
	int32_t AddMeshData(SubMesh* meshData);

	int32_t GetMeshDataId(SubMesh* meshData);

	RenderMeshBase* GetMeshData(int32_t meshId);

	int32_t AddMeshSkeletonLinkClusterData(SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId,const LString& skeletonUniqueName);

	int32_t GetMeshSkeletonLinkClusterDataId(SubMesh* meshData, const LString& skeletonUniqueName);

	MeshSkeletonLinkClusterBase* GetMeshSkeletonLinkClusterData(int32_t clusterId);

	int32_t AddAnimationInstanceMatrixData(const LString& animaInstanceUniqueName, const LArray<LMatrix4f> &allBoneMatrix);

	int32_t GetAnimationInstanceMatrixDataId(const LString& animaInstanceUniqueName);

	AnimationInstanceMatrix* GetAnimationInstanceMatrixData(int32_t animInstanceId);

	RHIView* GetSkinMatrixBuffer() { return mSkeletonResultBufferView.get(); }

	void Update();
private:
	LQueue<size_t> emptyMeshId;

	AssetSceneMemberDataPack<RenderMeshBase> meshPrimitiveBuffer;
	
	AssetSceneMemberDataPack<MeshSkeletonLinkClusterBase> mMeshSkeletonLinkClusterBuffer;
	
	AssetSceneMemberDataPack<AnimationInstanceMatrix> mAnimationInstanceMatrixBuffer;

	LString GetSubmeshName(SubMesh* meshData);

	LString GetClusterName(SubMesh* meshData, const LString& skeletonUniqueName);

	RHIResourcePtr mSkeletonResultBuffer;
	RHIViewPtr mSkeletonResultBufferView;
};

struct RENDER_API RenderObject
{
	int32_t           mMeshIndex = -1;
	int32_t           mSkinClusterIndex = -1;
	int32_t           mAnimInstanceIndex = -1;
	LMatrix4f*        mWorldMat;
	bool              mCastShadow = true;
	bool              mReceiveLight = true;
	bool              mReceiveShadow = true;
	uint64_t          mID = -1;
	MaterialInstance* mMaterial;
	LUnorderedMap<MeshRenderPass,uint64_t>  mAllCommandsIndex;
};

class RENDER_API RenderScene final : public RenderDataContainer
{
public:
	RenderScene();
	~RenderScene();;	

	void Render(FrameGraphBuilder* FG);
	//��ʱ�ȹ̶�mesh pass��ö��ȷ���������ٿ���ô��չ
	MeshRenderCommandsPacket mAllMeshDrawCommands[MeshRenderPass::AllNum];
public:
	
	void Init();

	DirectionLight* CreateMainDirLight();
	void DestroyMainDirLight(DirectionLight* val);
	PointLight* CreatePointLight();

	uint64_t CreateRenderObject(MaterialInstance* mat, SubMesh* meshData,bool castShadow, LMatrix4f* worldMat);

	uint64_t CreateRenderObjectDynamic(
		MaterialInstance* mat,
		SubMesh* meshData,
		const LUnorderedMap<LString, int32_t>& skeletonId,
		const LString& skeletonUniqueName,
		const LString& animaInstanceUniqueName,
		const LArray<LMatrix4f>& allBoneMatrix,
		bool castShadow,
		LMatrix4f* worldMat
	);

	void UpdateRenderObject(uint64_t roId);
	void SetRenderObjectMesh(uint64_t roId,SubMesh* meshData);
	void SetRenderObjectMeshSkletonCluster(uint64_t roId, SubMesh* meshData, const LUnorderedMap<LString, int32_t>& skeletonId, const LString& skeletonUniqueName);
	void SetRenderObjectAnimInstance(uint64_t roId, const LString& animaInstanceUniqueName, const LArray<LMatrix4f>& allBoneMatrix);
	void SetRenderObjectCastShadow(uint64_t roId, bool castShadow);
	void SetRenderObjectTransformRef(uint64_t roId, LMatrix4f* worldMat);
	void SetRenderObjectMaterial(uint64_t roId, MaterialInstance* mat);



	RenderView* CreateRenderView();
	void DestroyLight(Light* ro);
	void DestroyRenderObject(uint64_t ro);
	void DestroyRenderView(RenderView* renderView);

	bool mRenderable = true;
	bool mDrawGizmos = true;

	RenderMeshBase  mDebugMeshLineData;
	RenderMeshBase  mDebugMeshData;
public:
	void        SetSceneBufferDirty()             { mBufferDirty = true; }
	RenderView* GetRenderView(uint32_t idx) const { return mViews[idx]; };
	size_t      GetRenderObjectsCount() const     { return mRenderObjects.size(); }
	size_t      GetRenderViewNum() const          { return mViews.size(); }
	auto&       GetRenderObjects() const          { return mRenderObjects; };
	auto&       GetAllView() const                { return mViews; };


	ShaderCBuffer* mSceneParamsBuffer    = nullptr;
	ShaderCBuffer* mROIDInstancingBuffer = nullptr;

	//Gizmos Mesh
	//SubMesh*            mDebugMeshLine        = nullptr;
	//SubMesh*            mDebugMesh            = nullptr;

	//�Ȳ���Culling������Ӧ�ý���View����Culling������ID����
	
	RHIResourcePtr      mIDInstanceBuffer;

	//Main�����
	DirectionLight*     mMainDirLight   = nullptr;
	//Point Lights
	LArray<PointLight*> mPointLights;
	//������
	LVector4f           mAmbientColor   = LVector4f(0.05, 0.05, 0.05, 1.0);
	//Skybox
	MaterialInstance*   mSkyboxMaterial = nullptr;

	AssetSceneData      mSceneDataGpu;
protected:
	void PrepareScene();
	void Debug();
private:
	bool            mBufferDirty = true;
	bool            mInit        = false;

	LQueue<uint64_t> mRoIndex;
	LUnorderedMap<uint64_t, RenderObject* > mRenderObjects;
	ViewArray       mViews;
};

//һ���򵥵�pass����Ro�Ļ��࣬��ʱ�����������Զ������ݵĹ���
class RoPassProcessorBase
{
	RenderScene* mScene;
	MeshRenderPass mPassType;
public:
	RoPassProcessorBase(RenderScene* scene, MeshRenderPass passType);

	virtual void AddRenderObject(RenderObject* renderObject) = 0;

	void BuildMeshRenderCommands(
		RenderObject* renderObject,
		MaterialInstance* materialInstance
	);
};

typedef RoPassProcessorBase* (*MeshPassProcessorCreateFunction)(RenderScene* scene, MeshRenderPass passType);

class RoPassProcessorManager
{
	LUnorderedMap<MeshRenderPass, MeshPassProcessorCreateFunction> mAllPassProcessor;
	static RoPassProcessorManager* mInstance;
	RoPassProcessorManager();
	void Init();
public:
	void RegistorPassProcessor(MeshRenderPass passType, MeshPassProcessorCreateFunction passProcessorGenerator);
	MeshPassProcessorCreateFunction GetPassProcessor(MeshRenderPass passType);
	
	static RoPassProcessorManager* GetInstance();
};

}
