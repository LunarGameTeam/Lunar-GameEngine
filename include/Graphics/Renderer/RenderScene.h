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
	//��ʱ�ȹ̶�mesh pass��ö��ȷ���������ٿ���ô��չ
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
