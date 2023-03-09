#include "Graphics/Renderer/MaterialInstance.h"


#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"


#include "Graphics/RHI/RHIResource.h"

#include "Core/Memory/PtrBinding.h"
#include "Graphics/RHI/RHIPipeline.h"


namespace luna::render
{

//优化 增量更新
PARAM_ID(MaterialBuffer);

RegisterTypeEmbedd_Imp(MaterialParam)
{
	cls->Ctor<MaterialParam>();
	cls->Property<&Self::mParamType>("type")
		.Serialize();
	cls->BindingProperty<&Self::mParamName>("param_name")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialParamInt)
{
	cls->Ctor<MaterialParamInt>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialParamFloat)
{
	cls->Ctor<MaterialParamFloat>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialParamFloat2)
{
	cls->Ctor<MaterialParamFloat2>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialParamFloat3)
{
	cls->Ctor<MaterialParamFloat3>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialParamFloat4)
{
	cls->Ctor<MaterialParamFloat4>();
	cls->BindingProperty<&MaterialParamFloat4::mValue>("value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}
RegisterTypeEmbedd_Imp(MaterialParamTexture2D)
{
	cls->Ctor<MaterialParamTexture2D>();
	cls->BindingProperty< &Self::mValue>("texture")
		.Serialize();
}

RegisterTypeEmbedd_Imp(MaterialParamTextureCube)
{
	cls->Ctor<MaterialParamTextureCube>();
	cls->BindingProperty< &Self::mValue>("texture")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialInstance)
{
	cls->Ctor<MaterialInstance>();

	cls->BindingProperty< &Self::mOverrideParams>("params")
		.Serialize();

	cls->BindingProperty< &Self::mMaterialTemplate>("material_asset")
		.Serialize();

	cls->Binding<MaterialInstance>();
	BindingModule::Get("luna")->AddType(cls);
}

MaterialInstance::MaterialInstance() :
	mOverrideParams(this),
	mTemplateParams(this)
{

}

RHIShaderBlob* MaterialInstance::GetShaderVS()
{
	return mMaterialTemplate->GetShaderAsset()->GetVertexShader().get();
}

RHIShaderBlob* MaterialInstance::GetShaderPS()
{
	return mMaterialTemplate->GetShaderAsset()->GetPixelShader().get();
}

ShaderAsset* MaterialInstance::GetShaderAsset()
{
	return mMaterialTemplate->GetShaderAsset();
}

void MaterialInstance::Init()
{
	if (mMaterialTemplate)
	{
		auto* shader = mMaterialTemplate->GetShaderAsset();
		mTemplateParams = mMaterialTemplate->GetTemplateParams();
		PARAM_ID(MaterialBuffer);
		if (shader->GetVertexShader()->HasBindPoint(ParamID_MaterialBuffer) || shader->GetPixelShader()->HasBindPoint(ParamID_MaterialBuffer))
		{
			RHICBufferDesc materialBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc(ParamID_MaterialBuffer);
			RHIBufferDesc desc;
			desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
			desc.mSize = materialBufferDesc.mSize;
			mCBuffer = sRenderModule->GetRenderContext()->CreateBuffer(desc);

			ViewDesc viewDesc;
			viewDesc.mViewType = RHIViewType::kConstantBuffer;
			viewDesc.mViewDimension = RHIViewDimension::BufferView;
			mCBufferView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
			mCBufferView->BindResource(mCBuffer);

		}
		UpdateParamsToBuffer();

		mBindingSet = sRenderModule->GetRenderContext()->CreateBindingset(shader->mLayout);

		PARAM_ID(_ClampSampler);
		PARAM_ID(_RepeatSampler);
	
		SetShaderInput(ParamID__ClampSampler, sRenderModule->GetRenderContext()->mClamp.mView);
		SetShaderInput(ParamID__RepeatSampler, sRenderModule->GetRenderContext()->mRepeat.mView);		
	}
}


void MaterialInstance::SetShaderInput(ShaderParamID id, RHIView* view)
{
	mInputs[id] = view;
	mBindingDirty = true;
}

const TPPtrArray<MaterialParam>& MaterialInstance::GeTemplateParams()
{
	return mTemplateParams;
}

void MaterialInstance::UpdateBindingSet()
{
	auto shader = mMaterialTemplate->GetShaderAsset();
	std::vector<BindingDesc> bindingDescs;
	if (mCBufferView)
		bindingDescs.emplace_back(shader->GetBindPoint(ParamID_MaterialBuffer), mCBufferView);
	for(auto& it : mInputs)
	{
		bindingDescs.emplace_back(shader->GetBindPoint(it.first), it.second);
	}
	mBindingSet->WriteBindings(bindingDescs);
}

void MaterialInstance::UpdateParamsToBuffer()
{
	auto& params = mTemplateParams;
	auto shader = mMaterialTemplate->GetShaderAsset();
	const RHICBufferDesc& matBufferDesc = shader->GetConstantBufferDesc(ParamID_MaterialBuffer);

	std::vector<byte> data;
	data.resize(matBufferDesc.mSize);


	bool hasCbuffer = mMaterialTemplate->GetShaderAsset()->HasBindPoint(ParamID_MaterialBuffer);
	
	for (auto& param : mTemplateParams)
	{
		switch (param->mParamType)
		{
		case MaterialParamType::Texture2D:
		{
			auto* texture = static_cast<MaterialParamTexture2D*>(param.Get());
			texture->mValue->Init();
			mInputs[(param->mParamName.Hash())] = texture->mValue->mView;
			continue;
		}
		break;
		case MaterialParamType::TextureCube:
		{
			auto* texture = static_cast<MaterialParamTextureCube*>(param.Get());
			texture->mValue->Init();
			mInputs[(param->mParamName.Hash())] = texture->mValue->GetView();
			continue;
		}
		break;
		}
		if (hasCbuffer)
		{
			auto it = matBufferDesc.mVars.find(param->mParamName.Hash());
			if ( it == matBufferDesc.mVars.end())
				continue;
			const CBufferVar& var = it->second;
			switch (param->mParamType)
			{
			case MaterialParamType::Int:
			{
				auto* p = static_cast<MaterialParamInt*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
				break;
			}
			case MaterialParamType::Float:
			{
				auto* p = static_cast<MaterialParamFloat*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
				break;
			}
			case MaterialParamType::Float2:
			{
				MaterialParamFloat3* p = static_cast<MaterialParamFloat3*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
				break;
			}
			case MaterialParamType::Float3:
			{
				MaterialParamFloat3* p = static_cast<MaterialParamFloat3*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
				break;
			}
			case MaterialParamType::Float4:
			{
				MaterialParamFloat4* p = static_cast<MaterialParamFloat4*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
				break;
			}
			}
		}
	}	
	if (mMaterialTemplate->GetShaderAsset()->HasBindPoint(ParamID_MaterialBuffer))
		sRenderModule->GetRenderContext()->UpdateConstantBuffer(mCBuffer, data.data(), data.size());

}


RHIBindingSetPtr MaterialInstance::GetBindingSet()
{
	if (mBindingDirty)
	{
		UpdateBindingSet();
		mBindingDirty = false;
	}
	return mBindingSet;
}

RHIPipelineState* MaterialInstance::GetPipeline(RHIVertexLayout* layout, const RenderPassDesc& passDesc)
{
	size_t hashResult = 0;
	boost::hash_combine(hashResult, layout->Hash());
	boost::hash_combine(hashResult, passDesc.Hash());
	
	auto it = mPipelineCaches.find(hashResult);
	if (it != mPipelineCaches.end())
		return it->second;

	Log("Graphics", "Create pipeline for:{}", GetShaderAsset()->GetAssetPath());
	RHIPipelineStateDesc desc = {};
	RenderPipelineStateDescGraphic& graphicDesc = desc.mGraphicDesc;
	desc.mType = RHICmdListType::Graphic3D;

	graphicDesc.mPipelineStateDesc.DepthStencilState.DepthEnable = mMaterialTemplate->IsDepthTestEnable();
	graphicDesc.mPipelineStateDesc.DepthStencilState.DepthWrite = mMaterialTemplate->IsDepthWriteEnable();
	graphicDesc.mPipelineStateDesc.RasterizerState.CullMode = mMaterialTemplate->GetCullMode();
	graphicDesc.mPipelineStateDesc.PrimitiveTopologyType = (RHIPrimitiveTopologyType)mMaterialTemplate->GetPrimitiveType();
	graphicDesc.mInputLayout = *layout;
	graphicDesc.mPipelineStateDesc.mVertexShader = GetShaderVS();
	graphicDesc.mPipelineStateDesc.mPixelShader = GetShaderPS();
	graphicDesc.mRenderPassDesc = passDesc;

	RHIBlendStateTargetDesc blend = {};
	desc.mGraphicDesc.mPipelineStateDesc.BlendState.RenderTarget.push_back(blend);
	auto pipeline = sRenderModule->GetRHIDevice()->CreatePipeline(desc);
	mPipelineCaches[hashResult] = pipeline;
	return pipeline;

}
}

