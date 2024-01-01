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
#include"Graphics/Asset/tinyddsloader.h"

namespace luna::graphics
{

RegisterTypeEmbedd_Imp(Texture2D)
{
	cls->Ctor<Texture2D>(); 
	cls->Binding<Texture2D>();
	cls->BindingMethod<&Texture2D::GetRHITexture>("get_rhi_texture");
	BindingModule::Luna()->AddType(cls);
};

RegisterTypeEmbedd_Imp(TextureCube)
{
	cls->Ctor<TextureCube>();

	cls->Binding<TextureCube>();
	BindingModule::Luna()->AddType(cls);
};

void Texture2D::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	Super::OnAssetFileRead(meta, file);
	static RenderModule* render = gEngine->GetTModule<RenderModule>();
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
		mDesc.Format = RHITextureFormat::R8G8B8A8_UNORM_SRGB;		
		mDataType = TextureMemoryType::WIC;
		mDataSize = w * h * 4;
		mDesc.ResHeapType = RHIHeapType::Default;
		mDesc.Width = w;
		mDesc.Height = h;
	}

	Init();
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
	if (mRHIRes)
		return;	
	mDesc.mImageUsage = RHIImageUsage::SampledBit;
	mRHIRes = sRenderModule->mRenderContext->CreateTexture(mDesc, (byte*)mData, mDataSize);

	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kTexture;
	viewDesc.mViewDimension = RHIViewDimension::TextureView2D;
	mView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mView->BindResource(mRHIRes);
	
	Release();
}

Texture2D::Texture2D()
{
	mDesc.Format = RHITextureFormat::R8G8B8A8_UNORM;
}

TextureCube::TextureCube()
{
}

void TextureCube::Init()
{
	if (mRHIRes)
		return;
	LArray<byte> image_data;
	switch(mDataType)
	{
	case TextureMemoryType::DDS:
	{
		tinyddsloader::DDSFile newfile;
		newfile.Load(mData, mDataSize);
		switch (newfile.GetFormat())
		{
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm:
			mDesc.Format = RHITextureFormat::R8G8B8A8_UNORM;
			break;
		case tinyddsloader::DDSFile::DXGIFormat::B8G8R8A8_UNorm:
			mDesc.Format = RHITextureFormat::FORMAT_B8G8R8A8_UNORM;
			break;
		case tinyddsloader::DDSFile::DXGIFormat::D24_UNorm_S8_UInt:
			mDesc.Format = RHITextureFormat::D24_UNORM_S8_UINT;
			break;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_Float:
			mDesc.Format = RHITextureFormat::R16G16B16A16_FLOAT;
			break;
		case tinyddsloader::DDSFile::DXGIFormat::R16G16B16A16_UNorm:
			mDesc.Format = RHITextureFormat::R16G16B16A16_UNORM;
			break;
		case tinyddsloader::DDSFile::DXGIFormat::R8G8B8A8_UNorm_SRGB:
			mDesc.Format = RHITextureFormat::R8G8B8A8_UNORM_SRGB;
			break;
		default:
			mDesc.Format = RHITextureFormat::FORMAT_UNKNOWN;
			assert(0);
			break;
		}
		mDesc.Width = newfile.GetWidth();
		mDesc.Height = newfile.GetHeight();
		mDesc.MipLevels = newfile.GetMipCount();
		size_t allSize = 0;
		for (int16_t eachArrayIndex = 0; eachArrayIndex < mDesc.DepthOrArraySize; ++eachArrayIndex)
		{
			for (int16_t eachMipIndex = 0; eachMipIndex < mDesc.MipLevels; ++eachMipIndex)
			{
				const tinyddsloader::DDSFile::ImageData* curData = newfile.GetImageData(eachMipIndex,eachArrayIndex);
				allSize += curData->m_memSlicePitch;
			}
		}
		image_data.resize(allSize);
		size_t copyOffset = 0;
		for (int16_t eachArrayIndex = 0; eachArrayIndex < mDesc.DepthOrArraySize; ++eachArrayIndex)
		{
			for (int16_t eachMipIndex = 0; eachMipIndex < mDesc.MipLevels; ++eachMipIndex)
			{
				const tinyddsloader::DDSFile::ImageData* curData = newfile.GetImageData(eachMipIndex,eachArrayIndex);
				memcpy(image_data.data() + copyOffset, curData->m_mem, curData->m_memSlicePitch);
				copyOffset += curData->m_memSlicePitch;
			}
		}
		break;
	}
	case TextureMemoryType::WIC:
	{
		assert(false);
	}
	}
	mRHIRes = sRenderModule->GetRenderContext()->CreateTexture(mDesc, image_data.data(), image_data.size());

	ViewDesc desc;
	desc.mViewType = RHIViewType::kTexture;
	desc.mViewDimension = RHIViewDimension::TextureViewCube;
	desc.mBaseMipLevel = 0;
	desc.mLevelCount = mDesc.MipLevels;
	desc.mLayerCount = 6;
	mResView = sRenderModule->GetRHIDevice()->CreateView(desc);
	mResView->BindResource(mRHIRes.get());

}

void TextureCube::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	Super::OnAssetFileRead(meta, file);
	mDesc.Dimension = RHIResDimension::Texture2D;
	mDesc.DepthOrArraySize = 6;
	mDesc.mType = ResourceType::kTexture;
	mDesc.mImageUsage = RHIImageUsage::SampledBit;
	mDesc.ResHeapType = RHIHeapType::Default;
	mDesc.Layout = RHITextureLayout::LayoutUnknown;
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
		assert(false);
	}
	Init();
}

void TextureCube::Release()
{

}

void TextureCube::OnLoad()
{
}

}
