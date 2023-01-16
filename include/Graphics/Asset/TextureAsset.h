#pragma once

#include "Core/Math/Math.h"
#include "Core/Asset/Asset.h"
#include "Core/Asset/JsonAsset.h"

#include "Core/Foundation/String.h"
#include "Graphics/RHI/RHIResource.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/RHI/RHIDescriptorHeap.h"
#include "Graphics/RHI/RHIDescriptor.h"


namespace luna::render
{
class RENDER_API ITexture
{
public:
	ITexture()
	{
	};

	ITexture(unsigned int w, unsigned int h, RHITextureFormat format) : m_width(w), m_height(h), m_fomat(format)
	{
	};
	virtual const byte* GetData() = 0;
	virtual size_t GetDataSize() = 0;
	virtual void Release();
	GET_SET_VALUE(RHITextureFormat, m_fomat, TextureDataFormat);

	GET_SET_VALUE(unsigned int, m_width, Width);
	GET_SET_VALUE(unsigned int, m_height, Height);

	virtual RHIView* GetView() { return nullptr; }

	RHIResource* GetRHITexture()
	{
		return mRes.get();
	};

	RHIResourcePtr& GetRHITextureShared()
	{
		return mRes;
	};
protected:
	unsigned          int m_width;
	unsigned          int m_height;
	RHIResourcePtr    mRes;
	RHITextureFormat  m_fomat;
	TextureMemoryType m_type;
};


class RENDER_API Texture2D : public Asset, public ITexture
{
	RegisterTypeEmbedd(Texture2D, Asset)

public:
	Texture2D()
	{
	}
	
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	const byte* GetData() override;
	size_t GetDataSize() override;
	void Release() override;
	RHIView* GetView() override { return mView.get(); }
	void Init();
	RHIViewPtr mView;
private:
	void CreateDescriptor();
	RHITextureDesc tex_load_desc;
	RHIResDesc tex_res_desc;
	const byte* m_data;
	size_t m_init_data_size;
	bool m_init = false;
};


class RENDER_API TextureCube : public JsonAsset, public ITexture
{
	RegisterTypeEmbedd(TextureCube, JsonAsset)
public:
	TextureCube();


	void OnLoad() override;

public:
	void Init();
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	const byte* GetData() override;
	size_t GetDataSize() override;
	void Release() override;
	RHIView* GetView() override {	return mResView; 	}

private:
	RHITextureDesc mTexDesc;
	RHIResDesc mBufferDesc;
	RHIViewPtr mResView;
	TPPtrArray<LBinaryAsset> mTextures;
	const byte* m_data;
	size_t m_init_data_size;
	bool m_init = false;
};
}
