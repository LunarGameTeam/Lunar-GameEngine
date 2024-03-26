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

class MaterialBaseTemplateAsset;
class RhiUniformBufferPack;
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

enum class MaterialPipelineType
{
	Compute,
	GraphDraw
};

class RENDER_API MaterialInstanceBase : public LObject
{
	RegisterTypeEmbedd(MaterialInstanceBase, LObject)
public:
	MaterialInstanceBase();

	virtual ~MaterialInstanceBase();

	void SetShaderInput(ShaderParamID id, RHIView* view);

	ShaderAsset* GetShaderAsset();

	void BindToPipeline(RHICmdList* cmdList);

	RHICBufferDesc GetConstantBufferDesc(ShaderParamID name);

	void UpdateBindingSet();

	virtual void SetAsset(MaterialBaseTemplateAsset* asset);

	MaterialBaseTemplateAsset* GetAsset() { return mAsset; };
protected:
	MaterialPipelineType mPipelineType;

	LMap<ShaderParamID, RHIViewPtr> mInputs;
	
	int32_t mFramIndex = 0;
	
	LArray<RHIBindingSetPtr> mBindingSets;

	MaterialBaseTemplateAsset* mAsset;

	LArray<bool> mBindingDirty;

	bool HasBindPoint(ShaderParamID id) const;

	RHIBindPoint GetBindPoint(ShaderParamID id) const;

	virtual void UpdateBindingSetImpl(std::vector<BindingDesc> &bindingDescs) {};
};

class RENDER_API MaterialInstanceComputeBase : public MaterialInstanceBase
{
	RegisterTypeEmbedd(MaterialInstanceComputeBase, MaterialInstanceBase)
public:
	MaterialInstanceComputeBase();

	virtual ~MaterialInstanceComputeBase();

	RHIShaderBlob* GetShaderCS();

	RHIPipelineState* GetPipeline();

	void SetAsset(MaterialBaseTemplateAsset* asset) override;
private:
	RHIPipelineStatePtr mPipeline;
};

class RENDER_API MaterialInstanceGraphBase : public MaterialInstanceBase
{
	RegisterTypeEmbedd(MaterialInstanceGraphBase, MaterialInstanceBase)
public:
	MaterialInstanceGraphBase();

	virtual ~MaterialInstanceGraphBase();

	RHIShaderBlob* GetShaderVS();

	RHIShaderBlob* GetShaderPS();

	RHIPipelineState* GetPipeline(RHIVertexLayout* layout, const RenderPassDesc& pass);
};

class RENDER_API MaterialInstance : public MaterialInstanceGraphBase
{
	RegisterTypeEmbedd(MaterialInstance, MaterialInstanceGraphBase)
public:
	MaterialInstance();
	~MaterialInstance();
public:
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

	const TPPtrArray<MaterialParam>& GeTemplateParams();

public:
	size_t mMaterialBufferSize = 0;

	RhiUniformBufferPack* mCBuffer;

	RHIViewPtr mCBufferView;

	TPPtrArray<MaterialParam> mOverrideParams;

	TPPtrArray<MaterialParam> mTemplateParams;
private:
	bool mReady = false;

	void UpdateBindingSetImpl(std::vector<BindingDesc>& bindingDescs) override;
};


}

namespace luna
{
REGISTER_ENUM_TYPE(graphics::MaterialParamType, int)
REGISTER_ENUM_TYPE(graphics::MaterialPipelineType, int)
}
