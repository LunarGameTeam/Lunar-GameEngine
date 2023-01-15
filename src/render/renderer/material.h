#pragma once
#include "render/render_config.h"

#include "core/memory/ptr_binding.h"
#include "core/foundation/container.h"

#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_shader.h"

#include "render/renderer/types.h"


namespace luna::render
{

struct PackedParams
{
	void Clear()
	{
		mParams.clear();
	}

	void PushShaderParam(ShaderParamID id, RHIView* view)
	{
		auto& it = mParams.emplace_back();
		it.first = id;
		it.second = view;
	}

	std::vector<std::pair<ShaderParamID, RHIViewPtr>> mParams;
};


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

	PackedParams* GetPackedParams();

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

namespace luna
{
REGISTER_ENUM_TYPE(render::MaterialParamType, int)
}
