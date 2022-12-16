#pragma once

#include "core/math/math.h"
#include "core/asset/asset.h"
#include "core/asset/json_asset.h"

#include "core/foundation/string.h"
#include "render/rhi/rhi_resource.h"

#include "render/render_config.h"
#include "../rhi/rhi_descriptor_heap.h"
#include "../rhi/rhi_descriptor.h"


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


class RENDER_API Texture2D : public LBasicAsset, public ITexture
{
	RegisterTypeEmbedd(Texture2D, LBasicAsset)

public:
	Texture2D()
	{
	}
	
	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
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


class RENDER_API TextureCube : public LJsonAsset, public ITexture
{
	RegisterTypeEmbedd(TextureCube, LJsonAsset)
public:
	TextureCube();

public:
	void Init();
	void OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file) override;
	const byte* GetData() override;
	size_t GetDataSize() override;
	void Release() override;
	RHIView* GetView() override {	return mResView; 	}

private:
	RHITextureDesc mTexDesc;
	RHIResDesc mBufferDesc;
	RHIViewPtr mResView;
	TSubPtrArray<LBinaryAsset> m_textures;
	const byte* m_data;
	size_t m_init_data_size;
	bool m_init = false;
};
}
