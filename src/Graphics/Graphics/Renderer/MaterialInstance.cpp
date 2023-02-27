#include "Graphics/Renderer/MaterialInstance.h"


#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"


#include "Graphics/RHI/RHIResource.h"

#include "Core/Memory/PtrBinding.h"


namespace luna::render
{


ShaderParamsBuffer::ShaderParamsBuffer(const RHICBufferDesc& cbDesc) :
	mVars(cbDesc.mVars)
{
	RHIBufferDesc desc;
	mData.resize(cbDesc.mSize);
	desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
	desc.mSize = cbDesc.mSize;
	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kConstantBuffer;
	viewDesc.mViewDimension = RHIViewDimension::BufferView;
	mRes = sRenderModule->GetRenderContext()->CreateBuffer(desc);
	mView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mView->BindResource(mRes);
}

ShaderParamsBuffer::ShaderParamsBuffer(RHIBufferUsage usage, uint32_t size)
{
	RHIBufferDesc desc;
	mData.resize(size);
	desc.mBufferUsage = usage;
	desc.mSize = size;
	mRes = sRenderModule->GetRenderContext()->CreateBuffer(desc);	
}

void ShaderParamsBuffer::Commit()
{
	sRenderModule->mRenderContext->UpdateConstantBuffer(mRes, mData.data(), mData.size() * sizeof(byte));
}

void PackedParams::PushShaderParam(ShaderParamID id, ShaderParamsBuffer* buffer)
{
	assert(buffer != nullptr);
	auto& it = mParams.emplace_back();
	it.first = id;
	it.second = buffer->mView;
	boost::hash_combine(mParamsHash, id);
	boost::hash_combine(mParamsHash, buffer->mView.get());
}
void PackedParams::PushShaderParam(ShaderParamID id, RHIViewPtr view)
{
	assert(view != nullptr);
	auto& it = mParams.emplace_back();
	it.first = id;
	it.second = view;
	boost::hash_combine(mParamsHash, id);
	boost::hash_combine(mParamsHash, view.get());
}


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
	mAllParams(this)
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

		auto& defaultParams = mMaterialTemplate->GetAllParams();
		for (TPPtr<MaterialParam>& p : defaultParams)
		{
			mAllParams.PushBack(p.Get());
			mParamIndexMap[p->mParamName] = mAllParams.Size() - 1;
		}

		auto* shader = mMaterialTemplate->GetShaderAsset();
		PARAM_ID(MaterialBuffer);
		if (shader->GetVertexShader()->HasBindPoint(ParamID_MaterialBuffer) || shader->GetPixelShader()->HasBindPoint(ParamID_MaterialBuffer))
		{
			RHICBufferDesc materialBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc(ParamID_MaterialBuffer);
			RHIBufferDesc desc;
			desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
			desc.mSize = materialBufferDesc.mSize;
			mParamsBuffer = sRenderModule->GetRenderContext()->CreateBuffer(desc);

			ViewDesc viewDesc;
			viewDesc.mViewType = RHIViewType::kConstantBuffer;
			viewDesc.mViewDimension = RHIViewDimension::BufferView;
			mParamsView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
			mParamsView->BindResource(mParamsBuffer);

		}
		UpdateParamsToBuffer();
	}
}



TPPtrArray<MaterialParam>& MaterialInstance::GetAllParams()
{
	return mAllParams;
}

void MaterialInstance::UpdateParamsToBuffer()
{
	PARAM_ID(MaterialBuffer)
	auto& params = mAllParams;
	RHICBufferDesc matBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc(ParamID_MaterialBuffer);
	std::vector<byte> data;
	data.resize(matBufferDesc.mSize);
	mMaterialParams.Clear();
	for (auto& param : params)
	{
		switch (param->mParamType)
		{
		case MaterialParamType::Texture2D:
		{
			MaterialParamTexture2D* texture = static_cast<MaterialParamTexture2D*>(param.Get());
			texture->mValue->Init();
			mMaterialParams.PushShaderParam(param->mParamName.Hash(), texture->mValue->mView);
			continue;
		}
		break;
		case MaterialParamType::TextureCube:
		{
			MaterialParamTextureCube* texture = static_cast<MaterialParamTextureCube*>(param.Get());
			texture->mValue->Init();
			mMaterialParams.PushShaderParam(param->mParamName.Hash(), texture->mValue->GetView());
			continue;
		}
		break;
		}
		if (mMaterialTemplate->GetShaderAsset()->HasBindPoint(ParamID_MaterialBuffer))
		{
			if (matBufferDesc.mVars.find(param->mParamName.Hash()) == matBufferDesc.mVars.end())
				continue;
			CBufferVar& var = matBufferDesc.mVars[param->mParamName.Hash()];
			switch (param->mParamType)
			{
			case MaterialParamType::Int:
			{
				MaterialParamInt* p = static_cast<MaterialParamInt*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
			}
			break;
			case MaterialParamType::Float:
			{
				MaterialParamFloat* p = static_cast<MaterialParamFloat*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
			}
			break;
			case MaterialParamType::Float2:
			{
				MaterialParamFloat3* p = static_cast<MaterialParamFloat3*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
			}
			break;
			case MaterialParamType::Float3:
			{
				MaterialParamFloat3* p = static_cast<MaterialParamFloat3*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
			}
			break;
			case MaterialParamType::Float4:
			{
				MaterialParamFloat4* p = static_cast<MaterialParamFloat4*>(param.Get());
				memcpy(data.data() + var.mOffset, &p->mValue, var.mSize);
			}
				break;

			}
		}
	}

	if (mMaterialTemplate->GetShaderAsset()->HasBindPoint(ParamID_MaterialBuffer))
		sRenderModule->GetRenderContext()->UpdateConstantBuffer(mParamsBuffer, data.data(), data.size());


}

PackedParams* MaterialInstance::GetPackedParams()
{
	return &mMaterialParams;
}

}

