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
�����material baseָ��һ����װ��device pipeline�Լ���ص���Դ�󶨵Ĺ��ܡ����������е�device pipeline(��Ⱦ������)����Ҫ����һ����Ӧ�Ĳ��ʲ��ܷ���ָ��
������Կ�Ҫ��Ҫ���и���
*/
//�������ʣ�ֻ����shader������������Լ���Դ����صĹ���
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

//������ʣ�����ͨ��������ʷ���һ��compute��device pipeline
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

//��Ⱦ���ʣ�����ͨ��������ʷ���һ��3d drawcall��device pipeline
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

//��׼����
/*
�����material template��һ��������Unity����Ⱦ�ò��ʣ�������һЩ����(����int��float������ͼ)����Щ����������instance�б��ⲿ�޸�
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
