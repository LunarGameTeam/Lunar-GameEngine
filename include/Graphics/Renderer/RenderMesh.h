#pragma once
#include "Graphics/RenderConfig.h"
#include "Graphics/RenderTypes.h"

#include "Core/Foundation/Container.h"
#include "Graphics/RHI/RHIResource.h"
#include "Graphics/RHI/RHIDescriptor.h"


namespace luna::graphics
{
	//这里标记了场景渲染器用到的所有的mesh相关的renderpass
	//enum MeshRenderPass
	//{
	//	DirectLightShadowDepthPass = 0,
	//	PointLightShadowDepthPass,
	//	LightingPass,
	//	AllNum,
	//};
	class RenderMeshBase : public RenderData
	{
	public:
		//几何体数据
		RenderAssetDataMesh* mMeshData;
		//渲染参数
		bool              mCastShadow = true;
		bool              mReceiveLight = true;
		bool              mReceiveShadow = true;
		TPPtr<MaterialInstance> mMaterial;
		RenderMeshBase() :mMaterial(nullptr) 
		{
		}
	};

}
