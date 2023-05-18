#pragma once

#include "Graphics/Asset/ShaderAsset.h"
#include "Core/Math/Math.h"
#include "Core/Foundation/String.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RenderModule.h"


namespace luna::render
{

RegisterTypeEmbedd_Imp(ShaderMacro)
{
	cls->Ctor<ShaderMacro>();
	cls->Property<&Self::mMacroName>("macro_name")
		.Serialize();
	cls->BindingProperty<&Self::mMacroValue>("macro_value")
		.Serialize();
	BindingModule::Luna()->AddType(cls);
}

RegisterTypeEmbedd_Imp(ShaderAsset)
{
	cls->Binding<Self>();;
	cls->Ctor<ShaderAsset>();
	BindingModule::Get("luna")->AddType(cls);
};

void ShaderAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	TextAsset::OnAssetFileRead(meta, file);
}

SharedPtr<LShaderInstance> ShaderAsset::GenerateShaderInstance(RHIShaderType shaderType,const LArray<ShaderMacro*>& shaderMacros)
{
	SharedPtr<LShaderInstance> newInstance = MakeShared<LShaderInstance>(shaderType, GetAssetPath());
	newInstance->Init(GetContent(), shaderMacros);
}

void LShaderInstance::Init(
	const LString& content,
	LArray<ShaderMacro*> shaderMacros
)
{
	if (!mInit)
	{
		RHIShaderDesc shaderDesc;
		shaderDesc.mContent = content;
		shaderDesc.mName = mShaderName;
		shaderDesc.mType = mType;
		switch (mType)
		{
			case luna::render::RHIShaderType::Vertex:
			{
				shaderDesc.mEntryPoint = "VSMain";
			}
			break;
			case luna::render::RHIShaderType::Pixel:
			{
				shaderDesc.mEntryPoint = "PSMain";
			}
			break;
			default:
			break;
		}
		mRhiShader = sRenderModule->GetRenderContext()->CreateShader(shaderDesc);
		mShaderCacheId = sRenderModule->GetRenderContext()->GetShaderId(mRhiShader);
		mInit = true;
	}
}
};

