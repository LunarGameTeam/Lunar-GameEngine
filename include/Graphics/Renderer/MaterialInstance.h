#pragma once
#include "Graphics/RenderConfig.h"
#include "Core/CoreMin.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIShader.h"

#include "Graphics/RenderTypes.h"
#include "Core/Foundation/Misc.h"


namespace luna::render
{

// Shader Buffer的封装，根据CBuffer的Desc创建，内部封装了RHIResource和RHIView
// 提供快捷设置数据和Commit接口
struct RENDER_API ShaderParamsBuffer : NoCopy
{
	ShaderParamsBuffer(const RHICBufferDesc& cbDesc);
	ShaderParamsBuffer(RHIBufferUsage usage, uint32_t size);


	template<typename T>
	void Set(const LString& name, const T& val, uint32_t arrayIdx = 0, size_t extra_offset = 0)
	{
		CBufferVar& var = mVars[name.Hash()];
		size_t elementSize = 0;
		if (var.mIsArray)
			elementSize = var.mElementSize;
		auto offset = var.mOffset + elementSize * arrayIdx;
		LUNA_ASSERT(offset + extra_offset + sizeof(T) < mData.size());
		*((T*)(mData.data() + offset + extra_offset)) = val;
	}

	template<typename T>
	void Set(size_t extra_offset, const T& val)
	{
		LUNA_ASSERT(extra_offset + sizeof(T) < mData.size());
		*((T*)(mData.data() + extra_offset)) = val;
	}

	void SetData(const LString& name, void* data, uint32_t dataSize)
	{
		auto offset = mVars[name.Hash()].mOffset;
		LUNA_ASSERT(offset + dataSize < mData.size());
		memcpy(mData.data() + offset, data, dataSize);
	}

	void SetData(uint32_t offset, void* data, uint32_t dataSize)
	{
		LUNA_ASSERT(offset + dataSize < mData.size());
		memcpy(mData.data() + offset, data, dataSize);
	}

	void Commit();

	LMap<size_t, CBufferVar> mVars;
	LArray<byte>             mData;
	RHIResourcePtr           mRes;
	RHIViewPtr               mView;
};

//把ShaderParam打包，Param名字 + Texture View，或者是放ShaderParamBuffer
struct RENDER_API PackedParams : NoCopy
{
	void Clear()
	{
		mParams.clear();
		mParamsHash = 0;
	}

	void PushShaderParam(ShaderParamID id, RHIView* view);
	void PushShaderParam(ShaderParamID id, ShaderParamsBuffer* view);

	size_t mParamsHash = 0;

	LArray<std::pair<ShaderParamID, RHIViewPtr>> mParams;
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
