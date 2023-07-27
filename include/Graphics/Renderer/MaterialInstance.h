#pragma once
#include "Graphics/RenderConfig.h"
#include "Core/CoreMin.h"

#include "Graphics/RHI/RHITypes.h"
#include "Graphics/RHI/RHIShader.h"

#include "Graphics/Renderer/MaterialParam.h"

#include "Graphics/RenderTypes.h"
#include "Core/Object/SharedObject.h"


namespace luna::graphics
{

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

class RENDER_API MaterialParamFloat2 : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamFloat2, MaterialParam)
public:
	MaterialParamFloat2()
	{
	};
	MaterialParamType mParamType = MaterialParamType::Float;

	LVector2f mValue;
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


class RENDER_API MaterialParamFloat4 : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamFloat4, MaterialParam)
public:
	MaterialParamFloat4()
	{
	};
	MaterialParamType mParamType = MaterialParamType::Float4;

	LVector4f mValue;
};

class RENDER_API MaterialParamTexture2D : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamTexture2D, MaterialParam)
public:
	MaterialParamTexture2D() 
	{
		mParamType = MaterialParamType::Texture2D;
	}

	SharedPtr<Texture2D> mValue;
};


class RENDER_API MaterialParamTextureCube : public MaterialParam
{
	RegisterTypeEmbedd(MaterialParamTextureCube, MaterialParam)
public:
	MaterialParamTextureCube()
	{
		mParamType = MaterialParamType::TextureCube;
	}

	SharedPtr<TextureCube> mValue;
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

	LUnorderedMap<LString, size_t> mParamIndexMap;

	void SetShaderInput(ShaderParamID id, RHIView* view);

	const TPPtrArray<MaterialParam>& GeTemplateParams();
	
	RHIBindingSetPtr GetBindingSet();
	RHIPipelineState* GetPipeline(RHIVertexLayout* layout, const RenderPassDesc& pass);
public:
	LMap<ShaderParamID, RHIViewPtr> mInputs;
	
	size_t mMaterialBufferSize = 0;

	RHIResourcePtr mCBuffer;
	RHIViewPtr mCBufferView;

	TPPtrArray<MaterialParam> mOverrideParams;
	TPPtrArray<MaterialParam> mTemplateParams;

	SharedPtr<MaterialTemplateAsset> mMaterialTemplate;

private:
	void UpdateBindingSet();

	RHIBindingSetPtr mBindingSet;
	bool mBindingDirty = true;
	bool mReady = false;

	RHIBindPoint GetBindPoint(ShaderParamID id) const;

	RHICBufferDesc GetConstantBufferDesc(ShaderParamID name);

	bool HasBindPoint(ShaderParamID id) const;
};
}

namespace luna
{
REGISTER_ENUM_TYPE(graphics::MaterialParamType, int)
}
