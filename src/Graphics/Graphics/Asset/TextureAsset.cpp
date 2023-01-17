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

void ITexture::Release()
{
}

void Texture2D::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	static RenderModule* render = gEngine->GetModule<RenderModule>();
	tex_res_desc.Dimension = RHIResDimension::Texture2D;
	tex_res_desc.mType = ResourceType::kTexture;
	auto dds_type_test = file->GetPath().Find(".dds");
	if (dds_type_test != std::string::npos)
	{
		m_data = new byte[file->GetData().size()];
		memcpy(const_cast<byte*>(m_data), file->GetData().data(),
		       file->GetData().size() * sizeof(file->GetData()[0]));
		m_type = TextureMemoryType::DDS;
		m_width = 0;
		m_height = 0;
		m_fomat = RHITextureFormat::FORMAT_UNKNOWN;
		m_init_data_size = file->GetData().size() * sizeof(file->GetData()[0]);
	}
	else
	{
		int w, h, n;
		m_data = (const byte*)stbi_load_from_memory(file->GetData().data(), (int)file->GetData().size(), &w, &h, &n, 4);
		m_fomat = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
		m_width = w;
		m_height = h;
		m_type = TextureMemoryType::WIC;
		m_init_data_size = w * h * 4;

		tex_res_desc.ResHeapType = RHIHeapType::Default;
		tex_res_desc.Width = w;
		tex_res_desc.Height = h;
		tex_res_desc.Format = m_fomat;
	}

	//Init();
}

const byte* Texture2D::GetData()
{
	return m_data;
}

size_t Texture2D::GetDataSize()
{
	return m_height * m_width * 4;
}

void Texture2D::Release()
{
	if (m_data)
	{
		stbi_image_free((void*)m_data);
		m_data = nullptr;
	}
	
}

void Texture2D::Init()
{
	if (m_init)
		return;
	m_init = true;	
	tex_res_desc.mImageUsage = RHIImageUsage::SampledBit;
	mRes = sRenderModule->mRenderDevice->CreateTexture(tex_load_desc, tex_res_desc, (byte*)m_data, m_init_data_size);

	ViewDesc viewDesc;
	viewDesc.mViewType = RHIViewType::kTexture;
	viewDesc.mViewDimension = RHIViewDimension::TextureView2D;
	mView = sRenderModule->GetRHIDevice()->CreateView(viewDesc);
	mView->BindResource(mRes);
	
	Release();
}

void Texture2D::CreateDescriptor()
{
}

TextureCube::TextureCube():
	mTextures(this)
{
	m_fomat = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
}

void TextureCube::Init()
{
	if (m_init)
		return;
	m_init = true;

	size_t data_size = 0;

	LArray<byte> init_datas;

	for (auto& texture : mTextures)
	{
		auto file = texture->GetData();
		int w, h, n;
		auto data = (const byte*)stbi_load_from_memory(file->GetData().data(), (int)file->GetData().size(), &w, &h, &n, 4);
		m_width = w;
		m_height = h;
		auto stride = w * h * 4;
		init_datas.resize(data_size + stride);
		memcpy(init_datas.data() + data_size, data, stride);
		data_size += stride;
		stbi_image_free((void*)data);
	}

	m_fomat = RHITextureFormat::FORMAT_R8G8BB8A8_UNORM;
	m_type = TextureMemoryType::WIC;

	mBufferDesc.ResHeapType = RHIHeapType::Default;
	mBufferDesc.mType = ResourceType::kTexture;
	mBufferDesc.Layout = RHITextureLayout::LayoutUnknown;
	mBufferDesc.Width = m_width;
	mBufferDesc.Height = m_height;
	mBufferDesc.Format = m_fomat;
	mBufferDesc.Dimension = RHIResDimension::Texture2D;
	mBufferDesc.DepthOrArraySize = 6;
	mBufferDesc.mImageUsage = RHIImageUsage::SampledBit;
	
	mRes = sRenderModule->GetRenderDevice()->CreateTexture(mTexDesc, mBufferDesc, init_datas.data(), init_datas.size());

	ViewDesc desc;
	desc.mViewType = RHIViewType::kTexture;
	desc.mViewDimension = RHIViewDimension::TextureViewCube;
	desc.mBaseMipLevel = 0;
	desc.mLayerCount = 6;	
	mResView = sRenderModule->GetRHIDevice()->CreateView(desc);
	mResView->BindResource(mRes.get());

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

size_t TextureCube::GetDataSize()
{
	return m_height * m_width * 4;
}

void TextureCube::Release()
{
	free((void*)m_data);
}

void TextureCube::OnLoad()
{
}

}
