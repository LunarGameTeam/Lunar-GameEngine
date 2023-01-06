#pragma once
#include "material.h"

#include "core/foundation/container.h"
#include "core/asset/asset.h"
#include "core/asset/json_asset.h"
#include "core/memory/ptr_binding.h"

#include "render/render_config.h"
#include "render/renderer/render_device.h"
#include "render/asset/shader_asset.h"
#include "render/asset/texture_asset.h"

#include <d3d11.h>


namespace luna
{
namespace render
{
class MaterialTemplateAsset;


enum class MaterialParamType : uint8_t
{
	Int,
	Float,
	Float2,
	Float3,
	Float4,
	Texture2D,
	TextureCube,
};


class RENDER_API MaterialParam : public LObject
{
	RegisterTypeEmbedd(MaterialParam, LObject)
public:
	MaterialParam()
	{
	};
	MaterialParamType mParamType = MaterialParamType::Int;
	LString mParamName;
};


class RENDER_API MaterialParamInt : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamInt, MaterialParam)
public:
	MaterialParamInt()
	{
	};
	MaterialParamType mParamType = MaterialParamType::Int;
	int mValue;
};


class RENDER_API MaterialParamFloat : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamFloat, MaterialParam)
public:
	MaterialParamFloat()
	{
	};
	MaterialParamType mParamType = MaterialParamType::Float;

	float mValue;
};


class RENDER_API MaterialParamFloat3 : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamFloat3, MaterialParam)
public:
	MaterialParamFloat3()
	{
	};
	MaterialParamType mParamType = MaterialParamType::Float3;

	LVector3f mValue;
};


class RENDER_API MaterialParamTexture2D : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamTexture2D, MaterialParam)
public:
	MaterialParamTexture2D() :
		mValue(this)
	{
		mParamType = MaterialParamType::Texture2D;
	}

	TPPtr<Texture2D> mValue;
};


class RENDER_API MaterialParamTextureCube : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamTextureCube, MaterialParam)
public:
	MaterialParamTextureCube() :
		mValue(this)
	{
		mParamType = MaterialParamType::TextureCube;
	}

	TPPtr<TextureCube> mValue;
};


class RENDER_API MaterialInstance : public LObject
{
	RegisterTypeEmbedd(MaterialInstance, LObject)
public:
	MaterialInstance();

public:
	RHIShaderBlob* GetShaderVS();
	RHIShaderBlob* GetShaderPS();
	ShaderAsset* GetShaderAsset();

	bool Ready()
	{
		if (!mReady)
			Init();
		mReady = true;
		return mReady;
	}

	void Init();
	void UpdateParamsToBuffer();

	bool mReady = false;
	LUnorderedMap<LString, size_t> mParamIndexMap;
	TPPtrArray<MaterialParam>& GetAllParams();

	PackedParams* GetPackedParams() { return &mMaterialParams; }
public:
	PackedParams mMaterialParams;
	size_t mMaterialBufferSize = 0;
	RHIResourcePtr mParamsBuffer;
	RHIViewPtr mParamsView;

	TPPtrArray<MaterialParam> mOverrideParams;
	TPPtrArray<MaterialParam> mAllParams;

	TPPtr<MaterialTemplateAsset> mMaterialTemplate;

	
};
}


REGISTER_ENUM_TYPE(render::MaterialParamType, int)
}
