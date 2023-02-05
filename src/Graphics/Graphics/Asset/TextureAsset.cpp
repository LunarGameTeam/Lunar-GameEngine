#pragma once

#include "Graphics/Asset/TextureAsset.h"
#include "Core/Math/Math.h"
#include "Core/Memory/PtrBinding.h"
#include "Core/Foundation/String.h"
#include "Core/Framework/LunaCore.h"

#include "Graphics/RenderModule.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


namespace luna::render
{

RegisterTypeEmbedd_Imp(Texture2D)
{
	cls->Ctor<Texture2D>(); 
	cls->Binding<Texture2D>();
	BindingModule::Luna()->AddType(cls);
};

RegisterTypeEmbedd_Imp(TextureCube)
{
	cls->Ctor<TextureCube>();
	
	cls->Property<&Self::mTextures>("textures")
		.Serialize();

	cls->Binding<TextureCube>();
	BindingModule::Luna()->AddType(cls);
};

void Texture2D::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	Super::OnAssetFileRead(meta, file);
	static RenderModule* render = gEngine->GetModule<RenderModule>();
	mDesc.Dimension = RHIResDimension::Texture2D;
	mDesc.mType = ResourceType::kTexture;
	auto dds_type_test = file->GetPath().Find(".dds");
	if (dds_type_test != std::string::npos)
	{
		mData = new byte[file->GetData().size()];
		memcpy(const_cast<byte*>(mData), file->GetData().data(),
		       file->GetData().size() * sizeof(file->GetData()[0]));
		mDataType = TextureMemoryType::DDS;
		mDesc.Format = RHITextureFormat::FORMAT_UNKNOWN;
		mDataSize = file->GetData().size() * sizeof(file->GetData()[0]);
	}
	else
	{
		int w, h, n;
		mData = (const byte*)stbi_load_from_memory(file->GetData().data(), (int)file->GetData().size(), &w, &h, &n, 4);
		mDesc.Format = RHITextureFormat::R8G8BB8A8_UNORN;		
		mDataType = TextureMemoryType::WIC;
		mDataSize = w * h * 4;
		mDesc.ResHeapType = RHIHeapType::Default;
		mDesc.Width = w;
		mDesc.Height = h;
	}

	//Init();
}

const byte* Texture2D::GetData()
{
	return mData;
}

void Texture2D::Release()
{
	if (mData)
	{
		stbi_image_free((void*)mData);
		mData = nullptr;
	}
	
}

void Texture2D::Init()
{
	RHITextureDesc desc;
	if (mRHIRes)
		return;	
	mDesc.mImageUsage = RHIImageUsage::SampledBit;
	mRHIRes = sRenderModule->mRenderContext->CreateTexture(desc, mDesc, (byte*)mData, mDataSize);

	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kTexture;
	viewDesc.mViewDimension = RHIViewDimension::TextureView2D;
	mView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mView->BindResource(mRHIRes);
	
	Release();
}

Texture2D::Texture2D()
{
	mDesc.Format = RHITextureFormat::R8G8BB8A8_UNORN;
}

TextureCube::TextureCube():
	mTextures(this)
{
}

void TextureCube::Init()
{
	if (mRHIRes)
		return;

	size_t dataSize = 0;

	RHITextureDesc mTexDesc;
	LArray<byte> datas;

	for (auto& texture : mTextures)
	{
		auto file = texture->GetFileData();
		int w, h, n;
		auto data = (const byte*)stbi_load_from_memory(file->GetData().data(), (int)file->GetData().size(), &w, &h, &n, 4);
		auto stride = w * h * 4;
		datas.resize(dataSize + stride);
		mDesc.Width = w;
		mDesc.Height = h;		
		memcpy(datas.data() + dataSize, data, stride);
		dataSize += stride;
		stbi_image_free((void*)data);
	}

	mDesc.Format = RHITextureFormat::R8G8BB8A8_UNORN;
	mDataType = TextureMemoryType::WIC;

	mDesc.ResHeapType = RHIHeapType::Default;
	mDesc.mType = ResourceType::kTexture;
	mDesc.Layout = RHITextureLayout::LayoutUnknown;
	mDesc.Width = mDesc.Width;
	mDesc.Height = mDesc.Height;
	mDesc.Format = mDesc.Format;
	mDesc.Dimension = RHIResDimension::Texture2D;
	mDesc.DepthOrArraySize = 6;
	mDesc.mImageUsage = RHIImageUsage::SampledBit;
	
	mRHIRes = sRenderModule->GetRenderContext()->CreateTexture(mTexDesc, mDesc, datas.data(), datas.size());

	ViewDesc desc;
	desc.mViewType = RHIViewType::kTexture;
	desc.mViewDimension = RHIViewDimension::TextureViewCube;
	desc.mBaseMipLevel = 0;
	desc.mLayerCount = 6;	
	mResView = sRenderModule->GetRHIDevice()->CreateView(desc);
	mResView->BindResource(mRHIRes.get());

}

void TextureCube::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	JsonAsset::OnAssetFileRead(meta, file);
	Init();
}

const byte* TextureCube::GetData()
{
	return nullptr;
}

void TextureCube::Release()
{

}

void TextureCube::OnLoad()
{
}

}
