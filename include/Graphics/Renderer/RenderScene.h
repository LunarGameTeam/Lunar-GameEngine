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
class AssetSceneData
{
public:
	LUnorderedMap<LString, ShaderParamSceneBuffer> materialBuffer;
	//mesh现在先只添加不删除，有时间再处理ro离开场景怎么删除对应的scene资源的问题
	int32_t AddMeshData(SubMesh* meshData);

	int32_t GetMeshDataId(SubMesh* meshData);

	RenderMeshBase* GetMeshData(int32_t meshId);
private:
	LQueue<size_t> emptyMeshId;

	LUnorderedMap<LString, int32_t> meshPrimitiveName;

	LUnorderedMap<int32_t, RenderMeshBase> meshPrimitiveBuffer;

	LString GetSubmeshName(SubMesh* meshData);
};

struct RENDER_API RenderObject
{
	int32_t           mMeshIndex;
	LMatrix4f*        mWorldMat;
	bool              mCastShadow = true;
	bool              mReceiveLight = true;
	bool              mReceiveShadow = true;
	uint64_t          mID;
	MaterialInstance* mMaterial;
	LUnorderedMap<MeshRenderPass,uint64_t>  mAllCommandsIndex;
};

class RENDER_API RenderScene final : public RenderDataContainer
{
public:
	RenderScene();
	~RenderScene();;	

	void Render(FrameGraphBuilder* FG);
	//暂时先固定mesh pass在枚举确定，后面再看怎么拓展
	MeshRenderCommandsPacket mAllMeshDrawCommands[MeshRenderPass::AllNum];
public:
	
	void Init();

	DirectionLight* CreateMainDirLight();
	void DestroyMainDirLight(DirectionLight* val);
	PointLight* CreatePointLight();

	uint64_t CreateRenderObject(MaterialInstance* mat, SubMesh* meshData,bool castShadow, LMatrix4f* worldMat);
	void UpdateRenderObject(uint64_t roId);
	void SetRenderObjectMesh(uint64_t roId,SubMesh* meshData);
	void SetRenderObjectCastShadow(uint64_t roId, bool castShadow);
	void SetRenderObjectTransformRef(uint64_t roId, LMatrix4f* worldMat);
	void SetRenderObjectMaterial(uint64_t roId, MaterialInstance* mat);



	RenderView* CreateRenderView();
	void DestroyLight(Light* ro);
	void DestroyRenderObject(uint64_t ro);
	void DestroyRenderView(RenderView* renderView);

	bool mRenderable = true;
	bool mDrawGizmos = true;
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
	SubMesh*            mDebugMeshLine        = nullptr;
	SubMesh*            mDebugMesh            = nullptr;

	//先不做Culling，这里应该交给View进行Culling并进行ID更新
	
	RHIResourcePtr      mIDInstanceBuffer;

	//Main方向光
	DirectionLight*     mMainDirLight   = nullptr;
	//Point Lights
	LArray<PointLight*> mPointLights;
	//环境光
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

//一个简单的pass处理Ro的基类，暂时不考虑排序及自定义数据的功能
class RoPassProcessorBase
{
	RenderScene* mScene;
	MeshRenderPass mPassType;
public:
	RoPassProcessorBase(RenderScene* scene, MeshRenderPass passType);

	virtual void AddRenderObject(RenderObject* renderObject) = 0;

	void BuildMeshRenderCommands(
		RenderObject* renderObject,
		const LString& materialAsset
	);

	void BuildMeshRenderCommands(
		RenderObject* renderObject,
		MaterialTemplateAsset* materialAsset
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
