#include "material.h"
#include "core/asset/asset.h"
#include "core/asset/asset_module.h"
#include "render/renderer/material.h"

#include "render/asset/texture_asset.h"
#include "render/render_module.h"
#include "render/asset/material_template.h"
#include "render/rhi/rhi_resource.h"


namespace luna::render
{
RegisterTypeEmbedd_Imp(MaterialParam)
{
	cls->Ctor<MaterialParam>();
	cls->Property<&Self::mParamType>("type");
	cls->Property<&Self::mParamName>("param_name");
}

RegisterTypeEmbedd_Imp(MaterialParamInt)
{
	cls->Ctor<MaterialParamInt>();
	cls->Property<&Self::mValue>("value");
}

RegisterTypeEmbedd_Imp(MaterialParamFloat)
{
	cls->Ctor<MaterialParamFloat>();
	cls->Property<&Self::mValue>("value");
}

RegisterTypeEmbedd_Imp(MaterialParamFloat3)
{
	cls->Ctor<MaterialParamFloat3>();
	cls->Property<&Self::mValue>("value");
}

RegisterTypeEmbedd_Imp(MaterialParamTexture2D)
{
	cls->Ctor<MaterialParamTexture2D>();
	cls->Property< &Self::mValue>("texture");
}

RegisterTypeEmbedd_Imp(MaterialParamTextureCube)
{
	cls->Ctor<MaterialParamTextureCube>();
	cls->Property< &Self::mValue>("texture");
}

RegisterTypeEmbedd_Imp(MaterialInstance)
{
	cls->Ctor<MaterialInstance>();
	cls->Property< &Self::mOverrideParams>("params");
	cls->Property< &Self::mMaterialTemplate>("material_asset");
	cls->Binding<MaterialInstance>();
	LBindingModule::Get("luna")->AddType(cls);
}

MaterialInstance::MaterialInstance() :
	mOverrideParams(this),
	mMaterialTemplate(this),
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
		for (TSubPtr<MaterialParam>& p : defaultParams)
		{
			mAllParams.PushBack(p.Get());
			mParamIndexMap[p->mParamName] = mAllParams.Size() - 1;
		}

		auto* shader = mMaterialTemplate->GetShaderAsset();
		PARAM_ID(MaterialBuffer);
		if (shader->GetVertexShader()->HasBindPoint(ParamID_MaterialBuffer))
		{
			auto& materialBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc("MaterialBuffer");
			RHIBufferDesc desc;
			desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
			desc.mSize = materialBufferDesc.mBufferSize;
			mParamsBuffer = sRenderModule->GetRenderDevice()->CreateBuffer(desc);

			ViewDesc viewDesc;
			viewDesc.mViewType = RHIViewType::kConstantBuffer;
			viewDesc.mViewDimension = RHIViewDimension::BufferView;
			mParamsView = sRenderModule->GetRenderDevice()->CreateView(viewDesc);
			mParamsView->BindResource(mParamsBuffer);

		}
		UpdateParamsToBuffer();
	}
}



TSubPtrArray<MaterialParam>& MaterialInstance::GetAllParams()
{
	return mAllParams;
}

void MaterialInstance::UpdateParamsToBuffer()
{
	PARAM_ID(MaterialBuffer)
	auto& params = mAllParams;
	auto& matBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc("MaterialBuffer");
	std::vector<byte> data;
	data.resize(matBufferDesc.mBufferSize);
	mMaterialParams.Clear();
	for (TSubPtr<MaterialParam>& param : params)
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
			if (matBufferDesc.mVars.find(param->mParamName) == matBufferDesc.mVars.end())
				continue;
			ConstantBufferVar& var = matBufferDesc.mVars[param->mParamName];
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
				break;

			}
		}
	}

	if (mMaterialTemplate->GetShaderAsset()->HasBindPoint(ParamID_MaterialBuffer))
		sRenderModule->GetRenderDevice()->UpdateConstantBuffer(mParamsBuffer, data.data(), data.size());


}

}

