#include "Graphics/Renderer/MaterialInstance.h"
#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/RHI/RHIResource.h"
#include "Core/Memory/PtrBinding.h"
#include "Graphics/RHI/RHIPipeline.h"
#include "Graphics/RHI/RhiUtils/RHIResourceGenerateHelper.h"


namespace luna::graphics
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

RegisterTypeEmbedd_Imp(MaterialInstanceBase)
{
	cls->Ctor<MaterialInstanceBase>();

	cls->BindingProperty< &Self::mAsset>("material_asset")
		.Serialize();

	cls->Binding<MaterialInstanceBase>();
	BindingModule::Get("luna")->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialInstanceGraphBase)
{
	cls->Ctor<MaterialInstanceGraphBase>();

	cls->Binding<MaterialInstanceGraphBase>();
	BindingModule::Get("luna")->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialInstanceComputeBase)
{
	cls->Ctor<MaterialInstanceComputeBase>();

	cls->Binding<MaterialInstanceComputeBase>();
	BindingModule::Get("luna")->AddType(cls);
}

RegisterTypeEmbedd_Imp(MaterialInstance)
{
	cls->Ctor<MaterialInstance>();

	cls->BindingProperty< &Self::mOverrideParams>("params")
		.Serialize();

	cls->Binding<MaterialInstance>();
	BindingModule::Get("luna")->AddType(cls);
}

//基础材质实例
MaterialInstanceBase::MaterialInstanceBase()
{

};

MaterialInstanceBase::~MaterialInstanceBase()
{

};

void MaterialInstanceBase::SetShaderInput(ShaderParamID id, RHIView* view)
{
	if (mInputs[id] == view)
		return;
	mInputs[id] = view;
	for (int32_t frameId = 0; frameId < 2; ++frameId)
	{
		mBindingDirty[frameId] = true;
	}
}

ShaderAsset* MaterialInstanceBase::GetShaderAsset()
{
	return mAsset->GetShaderAsset();
}

void MaterialInstanceBase::BindToPipeline(RHICmdList* cmdList)
{
	int32_t lastFramIndex = (mFramIndex + 1) % 2;
	switch (mPipelineType)
	{
	case luna::graphics::MaterialPipelineType::Compute:
		cmdList->BindDesriptorSetExt(mBindingSets[lastFramIndex].get(), RHICmdListType::Compute);
		break;
	case luna::graphics::MaterialPipelineType::GraphDraw:
		cmdList->BindDesriptorSetExt(mBindingSets[lastFramIndex].get(), RHICmdListType::Graphic3D);
		break;
	default:
		assert(false);
		break;
	}
}

void MaterialInstanceBase::UpdateBindingSet()
{
	//这个update一旦更新了descriptor set，在drawcall期间是不能随便修改的，设计上应该还有再改进的地方，目前还只能靠上层保证每帧每个matinstance只更新一次
	if (mBindingDirty[mFramIndex])
	{
		auto shader = mAsset->GetShaderAsset();
		std::vector<BindingDesc> bindingDescs;
		for (auto& it : mInputs)
		{
			if (HasBindPoint(it.first))
			{
				bindingDescs.emplace_back(GetBindPoint(it.first), it.second);
			}
		}
		UpdateBindingSetImpl(bindingDescs);
		mBindingSets[mFramIndex]->WriteBindings(bindingDescs);
		mBindingDirty[mFramIndex] = false;
	}
	mFramIndex = (mFramIndex + 1) % 2;
}

void MaterialInstanceBase::SetAsset(MaterialBaseTemplateAsset* asset)
{
	mAsset = asset;
	mBindingSets.clear();
	mBindingDirty.clear();
	for (int32_t frameId = 0; frameId < 2; ++frameId)
	{
		RHIBindingSetPtr newBindingSet = sGlobelRhiResourceGenerator->CreateBindingsetByDefaultPool(mAsset->GetBindingSetLayout());
		mBindingSets.push_back(newBindingSet);
		mBindingDirty.push_back(true);
	}
}

RHIBindPoint MaterialInstanceBase::GetBindPoint(ShaderParamID id) const
{
	const LUnorderedMap<RHIShaderType, SharedPtr<LShaderInstance>>& allShader = mAsset->GetAllShader();
	for (auto& eachShader : allShader)
	{
		if (eachShader.second->HasBindPoint(id))
		{
			return eachShader.second->GetBindPoint(id);
		}
	}
	return RHIBindPoint();
};

bool MaterialInstanceBase::HasBindPoint(ShaderParamID id) const
{
	const LUnorderedMap<RHIShaderType, SharedPtr<LShaderInstance>> &allShader = mAsset->GetAllShader();
	for (auto& eachShader : allShader)
	{
		if(eachShader.second->HasBindPoint(id))
		{
			return true;
		}
	}
	return false;
}

RHICBufferDesc MaterialInstanceBase::GetConstantBufferDesc(ShaderParamID name)
{
	const LUnorderedMap<RHIShaderType, SharedPtr<LShaderInstance>>& allShader = mAsset->GetAllShader();
	for (auto& eachShader : allShader)
	{
		if (eachShader.second->GetRhiShader()->HasUniformBuffer(name))
		{
			return eachShader.second->GetRhiShader()->GetUniformBuffer(name);
		}
	}
	RHICBufferDesc empty;
	return empty;
}

//计算材质实例
MaterialInstanceComputeBase::MaterialInstanceComputeBase()
{
	mPipelineType = MaterialPipelineType::Compute;
};

MaterialInstanceComputeBase::~MaterialInstanceComputeBase()
{

};

void MaterialInstanceComputeBase::SetAsset(MaterialBaseTemplateAsset* asset)
{
	mAsset = asset;
	RHIPipelineStateComputeDesc desc = {};
	for (auto& itor : mAsset->GetAllShader())
	{
		desc.mShaders.insert({ itor.first,itor.second->GetRhiShader().get() });
	}
	mPipeline = sGlobelRhiResourceGenerator->GetDevicePipelineGenerator()->CreatePipelineCompute(desc);
	mBindingSets.clear();
	mBindingDirty.clear();
	for (int32_t frameId = 0; frameId < 2; ++frameId)
	{
		RHIBindingSetPtr newBindingSet = sGlobelRhiResourceGenerator->CreateBindingsetByDefaultPool(mPipeline->GetLayout());
		mBindingSets.push_back(newBindingSet);
		mBindingDirty.push_back(true);
	}
}

RHIShaderBlob* MaterialInstanceComputeBase::GetShaderCS()
{
	return mAsset->GetShaderByType(RHIShaderType::Compute)->GetRhiShader().get();
}

RHIPipelineState* MaterialInstanceComputeBase::GetPipeline()
{
	return mPipeline.get();
}


//渲染材质实例
MaterialInstanceGraphBase::MaterialInstanceGraphBase()
{
	mPipelineType = MaterialPipelineType::GraphDraw;
};

MaterialInstanceGraphBase::~MaterialInstanceGraphBase()
{

};

RHIShaderBlob* MaterialInstanceGraphBase::GetShaderVS()
{
	return mAsset->GetShaderByType(RHIShaderType::Vertex)->GetRhiShader().get();
}

RHIShaderBlob* MaterialInstanceGraphBase::GetShaderPS()
{
	return mAsset->GetShaderByType(RHIShaderType::Pixel)->GetRhiShader().get();
}

RHIPipelineState* MaterialInstanceGraphBase::GetPipeline(RHIVertexLayout* layout, const RenderPassDesc& passDesc)
{
	MaterialGraphAsset* mMaterialTemplate = dynamic_cast<MaterialGraphAsset*>(mAsset);
	RHIPipelineStateGraphDrawDesc desc = {};

	desc.DepthStencilState.DepthEnable = mMaterialTemplate->IsDepthTestEnable();
	desc.DepthStencilState.DepthWrite = mMaterialTemplate->IsDepthWriteEnable();
	desc.RasterizerState.CullMode = mMaterialTemplate->GetCullMode();
	desc.PrimitiveTopologyType = (RHIPrimitiveTopologyType)mMaterialTemplate->GetPrimitiveType();
	for (auto& itor : mAsset->GetAllShader())
	{
		desc.mShaders.insert({ itor.first,itor.second->GetRhiShader().get() });
	}
	RHIBlendStateTargetDesc blend = {};
	desc.BlendState.RenderTarget.push_back(blend);

	return sGlobelRhiResourceGenerator->GetDevicePipelineGenerator()->CreatePipelineGraphic(desc, *layout, passDesc);
}


//标准材质实例
MaterialInstance::MaterialInstance() :
	mOverrideParams(this),
	mTemplateParams(this)
{

}
MaterialInstance::~MaterialInstance()
{
	if (mCBuffer != nullptr)
	{
		delete mCBuffer;
	}
}
void MaterialInstance::Init()
{
	MaterialTemplateAsset* materialTemplate = dynamic_cast<MaterialTemplateAsset*>(mAsset);
	mCBuffer = new RhiUniformBufferPack();
	if (materialTemplate)
	{
		mTemplateParams = materialTemplate->GetTemplateParams();
		PARAM_ID(MaterialBuffer);
		if (HasBindPoint(ParamID_MaterialBuffer))
		{
			RHICBufferDesc materialBufferDesc = GetConstantBufferDesc(ParamID_MaterialBuffer);
			sGlobelRhiResourceGenerator->GetDeviceUniformObjectGenerator()->CreateUniformBufferAndView(materialBufferDesc, *mCBuffer, mCBufferView);
		}
		UpdateParamsToBuffer();
		PARAM_ID(_ClampSampler);
		PARAM_ID(_RepeatSampler);
	
		SetShaderInput(ParamID__ClampSampler, sGlobelRhiResourceGenerator->GetClampSamper().mView);
		SetShaderInput(ParamID__RepeatSampler, sGlobelRhiResourceGenerator->GetRepeatSamper().mView);
	}
}

const TPPtrArray<MaterialParam>& MaterialInstance::GeTemplateParams()
{
	return mTemplateParams;
}

void MaterialInstance::UpdateBindingSetImpl(std::vector<BindingDesc>& bindingDescs)
{
	if (mCBufferView)
		bindingDescs.emplace_back(GetBindPoint(ParamID_MaterialBuffer), mCBufferView);
}

void MaterialInstance::UpdateParamsToBuffer()
{
	auto& params = mTemplateParams;
	auto shader = mAsset->GetShaderAsset();
	const RHICBufferDesc& matBufferDesc = GetConstantBufferDesc(ParamID_MaterialBuffer);

	std::vector<byte> data;
	data.resize(matBufferDesc.mSize);


	bool hasCbuffer = HasBindPoint(ParamID_MaterialBuffer);
	
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
	if (HasBindPoint(ParamID_MaterialBuffer))
	{
		//todo：切换成GPU buffer
		//void* dst = mCBuffer.Map();
		//memcpy(dst, data.data(), data.size());
		//mCBuffer->Unmap();
	}
}

}

