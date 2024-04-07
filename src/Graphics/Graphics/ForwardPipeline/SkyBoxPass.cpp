#include "Graphics/ForwardPipeline/SkyBoxPass.h"
#include "Graphics/ForwardPipeline/ForwardRenderData.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"
#include "Graphics/Renderer/RenderContext.h"
namespace luna::graphics
{
	PARAM_ID(_SkyTex);
	bool SkyBoxPassGenerator::CheckRenderObject(const RenderObject* curRo) const
	{
		return false;
	}

	SkyBoxPassGenerator::SkyBoxPassGenerator()
	{
		mSkyBoxMeshAsset = sAssetModule->LoadAsset<MeshAsset>("/assets/built-in/Geometry/InsideSphere.lmesh");
		mSkyboxRenderMesh = mSkyBoxMeshAsset->GetSubMeshAt(0)->GetRenderMeshData();
		mSkyBoxMtlAsset = sAssetModule->LoadAsset<MaterialGraphAsset>("/assets/built-in/Skybox/Skybox.mat");
		mSkyBoxDefaultMtlInstance = dynamic_cast<MaterialInstanceGraphBase*>(mSkyBoxMtlAsset->GetDefaultInstance());
	};

	void SkyBoxPassGenerator::AddPassNode(FrameGraphBuilder* builder, RenderView* view)
	{
		if (view->mViewType != RenderViewType::SceneView)
		{
			return;
		}
		RenderScene* renderScene = view->mOwnerScene;
		FGGraphDrawNode* node = builder->AddGraphDrawPass("SkyBox");
		SceneRenderData* sceneRenderData = renderScene->RequireData<SceneRenderData>();
		if (sceneRenderData->mEnvTex == nullptr)
		{
			sceneRenderData->mEnvTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>("/assets/built-in/Textures/Cubemap.dds"));
			sceneRenderData->mIrradianceTex = LSharedPtr<TextureCube>(sAssetModule->LoadAsset<TextureCube>("/assets/built-in/Textures/IrradianceMap.dds"));
			sceneRenderData->mLUTTex = LSharedPtr<Texture2D>(sAssetModule->LoadAsset<Texture2D>("/assets/built-in/Textures/brdf.dds"));
		}
		
		graphics::RenderViewParameterData* viewParamData = view->GetData<graphics::RenderViewParameterData>();
		//sceneRenderData->SetMaterialParameter(mSkyBoxDefaultMtlInstance);
		mSkyBoxDefaultMtlInstance->SetShaderInput(ParamID__SkyTex, sceneRenderData->mEnvTex->GetView());
		PARAM_ID(_ClampSampler);
		mSkyBoxDefaultMtlInstance->SetShaderInput(ParamID__ClampSampler, sGlobelRhiResourceGenerator->GetClampSamper().mView);
		viewParamData->SetMaterialParameter(mSkyBoxDefaultMtlInstance);
		ViewTargetData* viewRtData = view->RequireData<ViewTargetData>();
		viewRtData->GenerateOpaqueResultRenderTarget(builder, node,true,true);

		node->ExcuteFunc([this](FrameGraphBuilder* builder, FGNode& node, RHICmdList* cmdlist)
			{
				mSkyBoxDefaultMtlInstance->UpdateBindingSet();
				sGlobelRenderCommondEncoder->DrawMesh(cmdlist,this->mSkyboxRenderMesh, this->mSkyBoxDefaultMtlInstance);
			});
	};
}

