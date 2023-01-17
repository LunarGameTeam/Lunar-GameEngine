#include "Graphics/Renderer/MaterialInstance.h"


#include "Graphics/RenderModule.h"

#include "Graphics/Asset/MaterialTemplate.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"


#include "Graphics/RHI/RHIResource.h"

#include "Core/Memory/PtrBinding.h"


namespace luna::render
{
RegisterTypeEmbedd_Imp(MaterialParam)
{
	cls->Ctor<MaterialParam>();
	cls->Property<&Self::mParamType>("type")
		.Serialize();
	cls->BindingProperty<&Self::mParamName>("param_name")
		.Serialize();
}

RegisterTypeEmbedd_Imp(MaterialParamInt)
{
	cls->Ctor<MaterialParamInt>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
}

RegisterTypeEmbedd_Imp(MaterialParamFloat)
{
	cls->Ctor<MaterialParamFloat>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
}

RegisterTypeEmbedd_Imp(MaterialParamFloat3)
{
	cls->Ctor<MaterialParamFloat3>();
	cls->BindingProperty<&Self::mValue>("value")
		.Serialize();
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
		for (TPPtr<MaterialParam>& p : defaultParams)
		{
			mAllParams.PushBack(p.Get());
			mParamIndexMap[p->mParamName] = mAllParams.Size() - 1;
		}

		auto* shader = mMaterialTemplate->GetShaderAsset();
		PARAM_ID(MaterialBuffer);
		if (shader->GetVertexShader()->HasBindPoint(ParamID_MaterialBuffer) || shader->GetPixelShader()->HasBindPoint(ParamID_MaterialBuffer))
		{
			RHIConstantBufferDesc materialBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc("MaterialBuffer");
			RHIBufferDesc desc;
			desc.mBufferUsage = RHIBufferUsage::UniformBufferBit;
			desc.mSize = materialBufferDesc.mBufferSize;
			mParamsBuffer = sRenderModule->GetRenderDevice()->CreateBuffer(desc);

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
	RHIConstantBufferDesc matBufferDesc = mMaterialTemplate->GetShaderAsset()->GetConstantBufferDesc("MaterialBuffer");
	std::vector<byte> data;
	data.resize(matBufferDesc.mBufferSize);
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

PackedParams* MaterialInstance::GetPackedParams()
{
	return &mMaterialParams;
}

}

