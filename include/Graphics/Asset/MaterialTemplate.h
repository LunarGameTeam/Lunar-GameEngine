#pragma once
#include "Core/CoreMin.h"

#include "Graphics/RenderConfig.h"
#include "Graphics/Asset/ShaderAsset.h"
#include "Graphics/Asset/TextureAsset.h"
#include "Graphics/Renderer/MaterialInstance.h"



namespace luna::graphics
{
class MaterialInstanceBase;
/*
这里的material base指代一个封装了device pipeline以及相关的资源绑定的功能。引擎里所有的device pipeline(渲染或计算等)都需要创建一个对应的材质才能发起指令
后面可以看要不要进行改名
*/
//基础材质，只包含shader创建，宏变体以及资源绑定相关的功能
class RENDER_API MaterialBaseTemplateAsset : public JsonAsset
{
	RegisterTypeEmbedd(MaterialBaseTemplateAsset, JsonAsset)
public:
	MaterialBaseTemplateAsset();

	virtual ~MaterialBaseTemplateAsset() {};

	ShaderAsset* GetShaderAsset() { return mShaderAsset.get(); }

	void OnLoad() override;

	RHIBindingSetLayout* GetBindingSetLayout() { return mLayout.get(); }

	LShaderInstance* GetShaderByType(RHIShaderType type);

	const LUnorderedMap<RHIShaderType, SharedPtr<LShaderInstance>>& GetAllShader() { return mShaderInstances; }

	virtual MaterialInstanceBase* CreateInstance() { return nullptr; };
protected:
	TPPtrArray<ShaderMacro> mTemplateMacros;

	SharedPtr<ShaderAsset> mShaderAsset;

	LUnorderedMap<RHIShaderType,SharedPtr<LShaderInstance>> mShaderInstances;

	RHIBindingSetLayoutPtr mLayout;

	virtual void CompileShaderAsset(ShaderAsset* curShader, const LArray<ShaderMacro*>& shaderMacros) {};

	void CompileShaderByType(
		ShaderAsset* curShader,
		const LArray<ShaderMacro*>& shaderMacros,
		RHIShaderType shaderType
	);

	void CompileShaderAndLayoutByType(
		ShaderAsset* curShader,
		const LArray<ShaderMacro*>& shaderMacros,
		const LArray<RHIShaderType>& shaderType
	);
};

//计算材质，可以通过这个材质发起一个compute的device pipeline
class RENDER_API MaterialComputeAsset : public MaterialBaseTemplateAsset
{
	RegisterTypeEmbedd(MaterialComputeAsset, MaterialBaseTemplateAsset)
public:
	MaterialComputeAsset();

	~MaterialComputeAsset() override;

	LShaderInstance* GetShaderComputeInstance();

	virtual MaterialInstanceBase* CreateInstance() override;
private:
	void CompileShaderAsset(ShaderAsset* curShader, const LArray<ShaderMacro*>& shaderMacros) override;
};

//渲染材质，可以通过这个材质发起一个3d drawcall的device pipeline
class RENDER_API MaterialGraphAsset : public MaterialBaseTemplateAsset
{
	RegisterTypeEmbedd(MaterialGraphAsset, MaterialBaseTemplateAsset)

public:
	MaterialGraphAsset();

	~MaterialGraphAsset() override;

	LShaderInstance* GetShaderVertexInstance();

	LShaderInstance* GetShaderPixelInstance();

	virtual MaterialInstanceBase* CreateInstance() override;

	MaterialInstanceBase* GetDefaultInstance() {
		if (!mDefaultInstance)
			mDefaultInstance = dynamic_cast<MaterialInstanceGraphBase*>(CreateInstance());
		return mDefaultInstance.Get();
	}

	RHIRasterizerCullMode GetCullMode();

	void SetCullMode(RHIRasterizerCullMode val);

	bool IsDepthWriteEnable();

	void SetDepthWriteEnable(bool val);

	bool IsDepthTestEnable();

	void SetDepthTestEnable(bool val);

	RHIPrimitiveTopologyType GetPrimitiveType() { return mPrimitiveType; }

	void SetPrimitiveType(RHIPrimitiveTopologyType val) { mPrimitiveType = val; }

private:
	RHIRasterizerCullMode mCullMode = RHIRasterizerCullMode::BackFace;

	bool mDepthTestEnable = true;

	bool mDepthWriteEnable = true;

	RHIPrimitiveTopologyType mPrimitiveType = RHIPrimitiveTopologyType::Triangle;

	TPPtr<MaterialInstanceGraphBase> mDefaultInstance;

	void CompileShaderAsset(ShaderAsset* curShader, const LArray<ShaderMacro*>& shaderMacros) override;
};

//标准材质
/*
这里的material template是一个类似于Unity的渲染用材质，附带了一些参数(比如int或float或者贴图)，这些参数可以在instance中被外部修改
*/
class RENDER_API MaterialTemplateAsset : public MaterialGraphAsset
{
	RegisterTypeEmbedd(MaterialTemplateAsset, MaterialGraphAsset)
public:
	MaterialTemplateAsset();

	TPPtrArray<MaterialParam>& GetTemplateParams();

	MaterialInstanceBase* CreateInstance() override;
private:
	TPPtrArray<MaterialParam> mTemplateParams;
};
}
