#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"
#include "Graphics/RHI/RHITypes.h"
#include "Core/Foundation/Misc.h"
#include "Graphics/Renderer/RenderData.h"

#include <functional>

namespace luna::render
{

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

	RenderObject* CreateRenderObject();
	RenderView* CreateRenderView();
	void DestroyLight(Light* ro);
	void DestroyRenderObject(RenderObject* ro);
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
	
protected:
	void PrepareScene();
	void Debug();
private:
	bool            mBufferDirty = true;
	bool            mInit        = false;
	ROArray         mRenderObjects;
	ViewArray       mViews;
};
}
