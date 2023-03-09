#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/MaterialTemplate.h"

#include "Core/Asset/AssetModule.h"

#include "Graphics/FrameGraph/FrameGraph.h"
#include "Graphics/FrameGraph/FrameGraphNode.h"
#include "Graphics/FrameGraph/FrameGraphResource.h"

#include "Graphics/Renderer/MaterialParam.h"

#include "Graphics/Renderer/MaterialParam.h"

#include "Graphics/Renderer/RenderScene.h"
#include "Graphics/Renderer/RenderView.h"
#include "Graphics/Renderer/RenderLight.h"
#include "Graphics/RenderModule.h"

#include "Graphics/ForwardPipeline/ForwardRenderData.h"

namespace luna::render
{

PARAM_ID(SceneBuffer);
PARAM_ID(ViewBuffer);
PARAM_ID(MaterialBuffer);

void PBRPreparePass(FrameGraphBuilder* builder, RenderView* view, RenderScene* renderScene)
{
	SceneRenderData* data = renderScene->RequireData<SceneRenderData>();
	FGResourceView* sceneView = nullptr;

	for (const auto& param : renderScene->mSkyboxMaterial->GeTemplateParams())
	{
		if (param->mParamType == MaterialParamType::TextureCube)
		{
			auto cube = (MaterialParamTextureCube*)(param.Get());
			data->mEnvTex = builder->BindExternalTexture(cube->mValue->GetRHIResource(), "EnvTex");
		}
	}
	if (data->mLUTTex)
		return;
	auto& node = builder->AddPass("LUTGenerate");

	data->mLUTTex = builder->CreateTexture(
		512, 512, 1, 1,
		RHITextureFormat::R16G16B16A16_UNORM,
		RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
		"LUT");
	sceneView = node.AddRTV(data->mLUTTex, RHIViewDimension::TextureView2D);
	node.SetColorAttachment(sceneView, LoadOp::kClear);

	static auto sLUTMaterial = sAssetModule->LoadAsset<MaterialTemplateAsset>("/assets/built-in/LUT.mat");
	static MaterialInstance* lutMatInstance = sLUTMaterial->GetDefaultInstance();
	if (lutMatInstance)
		lutMatInstance->Ready();
	node.ExcuteFunc([view, renderScene, sceneView](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
	{
		lutMatInstance->SetShaderInput(ParamID_SceneBuffer, renderScene->mSceneParamsBuffer->mView);
	device->DrawMesh(sRenderModule->mFullscreenMesh, lutMatInstance, nullptr);
	});

	

	static auto irrMat = sAssetModule->LoadAsset<MaterialTemplateAsset>(
		"/assets/built-in/IrradianceCube.mat");
	static auto cube = sAssetModule->LoadAsset<MeshAsset>("/assets/built-in/Geometry/Box.lmesh");
	static LArray<MaterialInstance*> iradMatInstance;
	iradMatInstance.push_back(irrMat->CreateInstance());
	iradMatInstance.push_back(irrMat->CreateInstance());
	iradMatInstance.push_back(irrMat->CreateInstance());
	iradMatInstance.push_back(irrMat->CreateInstance());
	iradMatInstance.push_back(irrMat->CreateInstance());
	iradMatInstance.push_back(irrMat->CreateInstance());
	for (auto it : iradMatInstance)
	{
		it->Ready();
	}
	if (data->mIrradianceTex)
		return;

	LArray<LMatrix4f> mViewMatrix;
	LArray<ShaderCBuffer*> sViewBuffers;
	if (sViewBuffers.size() == 0)
	{
		const auto& desc = sRenderModule->GetRenderContext()->mDefaultShader->GetConstantBufferDesc(LString("ViewBuffer").Hash());
		sViewBuffers.push_back(new ShaderCBuffer(desc));
		sViewBuffers.push_back(new ShaderCBuffer(desc));
		sViewBuffers.push_back(new ShaderCBuffer(desc));
		sViewBuffers.push_back(new ShaderCBuffer(desc));
		sViewBuffers.push_back(new ShaderCBuffer(desc));
		sViewBuffers.push_back(new ShaderCBuffer(desc));
		mViewMatrix.resize(6);
	}
	LQuaternion rotation[] =
	{
		LMath::FromEuler(LVector3f(0,90,0)),
		LMath::FromEuler(LVector3f(0,270,0)),
		LMath::FromEuler(LVector3f(-90,0,0)),
		LMath::FromEuler(LVector3f(90,0,0)),
		LMath::FromEuler(LVector3f(0,0,0)),
		LMath::FromEuler(LVector3f(0,180,0)),
	};
	LMatrix4f mProjMatrix;
	LMath::GenPerspectiveFovLHMatrix(mProjMatrix, (float)(M_PI / 2.0), 1, 0.01, 10);

	for (uint32_t idx = 0; idx < 6; idx++)
	{
		LTransform transform = LTransform::Identity();
		transform.rotate(rotation[idx]);
		mViewMatrix[idx] = transform.matrix().inverse();
		sViewBuffers[idx]->Set("cViewMatrix", mViewMatrix[idx]);
		sViewBuffers[idx]->Set("cProjectionMatrix", mProjMatrix);
	}
	for (uint32_t idx = 0; idx < 6; idx++)
	{
		sViewBuffers[idx]->Commit();
	}

	data->mIrradianceTex = builder->CreateTexture(
		128, 128, 6, 1,
		RHITextureFormat::R8G8B8A8_UNORM,
		RHIImageUsage::ColorAttachmentBit | RHIImageUsage::SampledBit,
		"Irradience");

	FGResourceView* envView = nullptr;
	for (uint32_t idx = 0; idx < 6; idx++)
	{
		FGResourceView* irradienceView = nullptr;
		auto& node = builder->AddPass("IrradienceCubeGenerate");

		irradienceView = node.AddRTV(data->mIrradianceTex, RHIViewDimension::TextureView2D, idx);
		envView = node.AddSRV(data->mEnvTex, RHIViewDimension::TextureViewCube, 0, 6);
		node.SetColorAttachment(irradienceView, LoadOp::kClear);

		node.ExcuteFunc([=](FrameGraphBuilder* builder, FGNode& node, RenderContext* device)
		{
			PARAM_ID(_EnvTex);
		PARAM_ID(ViewBuffer);
		SubMesh* cubeSubMesh = cube->GetSubMeshAt(0);
		for (const auto& param : renderScene->mSkyboxMaterial->GeTemplateParams())
		{
			if (param->mParamType == MaterialParamType::TextureCube)
			{
				auto cube = (MaterialParamTextureCube*)(param.Get());
				iradMatInstance[idx]->SetShaderInput(ParamID_ViewBuffer, sViewBuffers[idx]->mView);
				iradMatInstance[idx]->SetShaderInput(ParamID__EnvTex, envView->mRHIView);
				device->DrawMesh(cubeSubMesh, iradMatInstance[idx], nullptr);
				break;
			}
		}
		});
	}
}

}

