#pragma once
#include "render/pch.h"
#include "render/rhi/rhi_types.h"
#include "render/rhi/rhi_ptr.h"
#include "render/rhi/rhi_pch.h"


namespace luna::render
{

enum class SamplerFilter
{
	kAnisotropic,
	kMinMagMipLinear,
	kComparisonMinMagMipLinear,
};

enum class SamplerTextureAddressMode
{
	kWrap,
	kClamp
};

enum class SamplerComparisonFunc
{
	kNever,
	kAlways,
	kLess
};
struct SamplerDesc
{

	SamplerFilter filter;
	SamplerTextureAddressMode mode;
	SamplerComparisonFunc func;
};

struct RHIStaticSamplerDesc
{
	RHIRenderFilter        Filter           = RHIRenderFilter::FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
	RHITextureAddressMode  AddressU         = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
	RHITextureAddressMode  AddressV         = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
	RHITextureAddressMode  AddressW         = RHITextureAddressMode::TEXTURE_ADDRESS_MODE_WRAP;
	float                  MipLODBias       = 0.0;
	float                  MaxAnisotropy    = 1;
	RHIComparisionFunc     ComparisonFunc   = RHIComparisionFunc::FuncNever;
	LunarStaticBorderColor BorderColor      = LunarStaticBorderColor::STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	float                  MinLOD           = 0;
	float                  MaxLOD           = FLT_MAX;
	uint32_t               ShaderRegister   = 0;
	uint32_t               RegisterSpace    = 0;
	RHIShaderVisibility    ShaderVisibility = RHIShaderVisibility::Visibility_All;
};

struct RHIBindPoint
{
	LString             mName             = "";
	RHIShaderVisibility mShaderVisibility = RHIShaderVisibility::Visibility_Vertex;//目前ShaderVisibility不生效
	RHIViewType         mViewType         = RHIViewType::Uniform_Invalid;
	uint32_t            mSlot             = 0;
	uint32_t            mSpace            = 0;
	uint32_t            mCount            = 1;
	uint32_t			mSize = 0;

	operator bool() const
	{
		return mName != 0 && mViewType != RHIViewType::Uniform_Invalid;
	}

	auto MakeTie() const
	{
		return std::tie(mShaderVisibility, mViewType, mSlot, mSpace, mCount);
	}
};

using BindKeyMap = std::map<std::pair<uint32_t, uint32_t>, RHIBindPoint>;

struct BindingDesc
{
	RHIBindPoint mBindTarget;
	RHIViewPtr mView;
	size_t mOffset = 0;
	auto MakeTie() const
	{
		return std::tie(mBindTarget, mView);
	}
};

template<typename T>
auto operator< (const T& l, const T& r)->std::enable_if_t<std::is_same_v<decltype(l.MakeTie() < r.MakeTie()), bool>, bool>
{
	return l.MakeTie() < r.MakeTie();
}

struct RHIRootSignatureParamDesc
{
	RHIDescriptorRangeType mRangeType       = RHIDescriptorRangeType::RangeTypeSrv;
	int32_t                mDescriptorNum;
	int32_t                mShaderRegister;
	int32_t                mRegisterSapce;
	RHIDescriptorRangeFlag mFlag            = RHIDescriptorRangeFlag::DESCRIPTOR_RANGE_FLAG_NONE;
	int32_t                mDescriptorRange;
	RHIShaderVisibility    mShaderVisiblity = RHIShaderVisibility::Visibility_All;
};


struct RHIRootSignatureDesc
 {
	LVector<RHIRootSignatureParamDesc> mParamDescs;
	LVector<RHIStaticSamplerDesc>      mStaticSamplerDesc;
	RHIRootSignatureFlag               mFlags = RHIRootSignatureFlag::ROOT_SIGNATURE_FLAG_NONE;
};

class RHIBindingSet : public RHIObject
{
public:
	virtual ~RHIBindingSet() = default;
	virtual void WriteBindings(const std::vector<BindingDesc>& bindings) = 0;
};

class RENDER_API RHIBindingSetLayout : public RHIObject
{
	RHIRootSignatureDesc m_root_signature_desc;
public:
	RHIBindingSetLayout() = default;

	virtual ~RHIBindingSetLayout() override
	{
	};
};
}
