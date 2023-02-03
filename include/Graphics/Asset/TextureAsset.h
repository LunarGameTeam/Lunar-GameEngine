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

class RENDER_API Texture2D : public LBinaryAsset
{
	RegisterTypeEmbedd(Texture2D, Asset)

public:
	Texture2D();
	
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;

	const byte* GetData();

	size_t GetDataSize()
	{
		return mDataSize;
	}

	

	RHIView* GetView() const 
	{
		return mView; 
	}
	void Init();

	RHIViewPtr mView;

	RHIResource* GetRHITexture()
	{
		return mRHIRes.get();
	};
	uint32_t GetWidth() const
	{
		return mDesc.Width;
	}
	uint32_t GetHeight() const
	{
		return mDesc.Height;
	}
	RHITextureFormat GetFormat() const
	{
		return mDesc.Format;
	}
	void Release();

private:	
	const             byte* mData = nullptr;
	size_t            mDataSize   = 0;
	RHIResDesc        mDesc;
	RHIResourcePtr    mRHIRes;
	TextureMemoryType mDataType;
};


class RENDER_API TextureCube : public JsonAsset
{
	RegisterTypeEmbedd(TextureCube, JsonAsset)
public:
	TextureCube();


	void OnLoad() override;

public:
	void Init();
	void OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file) override;
	const byte* GetData();
	size_t GetDataSize();
	void Release();
	RHIView* GetView() {	return mResView; 	}

private:
	RHIResDesc            mDesc;
	RHIResourcePtr        mRHIRes;
	RHIViewPtr            mResView;
	TextureMemoryType     mDataType;
	TPPtrArray<Texture2D> mTextures;
	size_t                mDataSize;
};
}
