#include "Graphics/RHI/DirectX12/DX12Pipeline.h"
#include "Core/Asset/AssetModule.h"
#include "Graphics/RHI/DirectX12/DX12Device.h"
#include "Graphics/RHI/DirectX12/DX12RenderPass.h"

using Microsoft::WRL::ComPtr;


namespace luna::graphics
{
DX12PipelineStateGraphic::DX12PipelineStateGraphic(
	LSharedPtr<RHIPipelineStateDescBase> psoDesc,
	const RHIVertexLayout& inputLayout,
	const RenderPassDesc& renderPassDesc
) : RHIPipelineStateGraphic(psoDesc, inputLayout, renderPassDesc)
{
	//InitPipeline(pso_desc);
}
static const char DX_VERTEX_ELEMENT_POSITION[] = "POSITION";
static const char DX_VERTEX_ELEMENT_BLENDINDEX[] = "BLENDINDEX";
static const char DX_VERTEX_ELEMENT_BLENDWEIGHT[] = "BLENDWEIGHT";
static const char DX_VERTEX_ELEMENT_NORMAL[] = "NORMAL";
static const char DX_VERTEX_ELEMENT_DIFFUSE[] = "DIFFUSE";
static const char DX_VERTEX_ELEMENT_TANGENT[] = "TANGENT";
static const char DX_VERTEX_ELEMENT_TEXCOORD[] = "TEXCOORD";
static const char DX_VERTEX_ELEMENT_INSTANCEMESSAGE[] = "INSTANCEMESSAGE";
const char* DX12PipelineStateGraphic::GetDx12ElementName(VertexElementUsage m_usage)
{
	switch (m_usage)
	{
	case luna::graphics::VertexElementUsage::UsagePosition:
		return DX_VERTEX_ELEMENT_POSITION;
		break;
	case luna::graphics::VertexElementUsage::UsageBlendIndex:
		return DX_VERTEX_ELEMENT_BLENDINDEX;
		break;
	case luna::graphics::VertexElementUsage::UsageBlendWeight:
		return DX_VERTEX_ELEMENT_BLENDWEIGHT;
		break;
	case luna::graphics::VertexElementUsage::UsageNormal:
		return DX_VERTEX_ELEMENT_NORMAL;
		break;
	case luna::graphics::VertexElementUsage::UsageDiffuse:
		return DX_VERTEX_ELEMENT_DIFFUSE;
		break;
	case luna::graphics::VertexElementUsage::UsageTangent:
		return DX_VERTEX_ELEMENT_TANGENT;
		break;
	case luna::graphics::VertexElementUsage::UsageColor:
		return DX_VERTEX_ELEMENT_TEXCOORD;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture0:
		return DX_VERTEX_ELEMENT_TEXCOORD;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture1:
		return DX_VERTEX_ELEMENT_TEXCOORD;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture2:
		return DX_VERTEX_ELEMENT_TEXCOORD;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture3:
		return DX_VERTEX_ELEMENT_TEXCOORD;
		break;
	case luna::graphics::VertexElementUsage::UsageInstanceMessage:
		return DX_VERTEX_ELEMENT_INSTANCEMESSAGE;
		break;
	default:
		return "";
		break;
	}
}

int32_t DX12PipelineStateGraphic::GetDx12ElementIndex(VertexElementUsage m_usage)
{
	switch (m_usage)
	{
	case luna::graphics::VertexElementUsage::UsagePosition:
	case luna::graphics::VertexElementUsage::UsageBlendWeight:
	case luna::graphics::VertexElementUsage::UsageNormal:
	case luna::graphics::VertexElementUsage::UsageDiffuse:
	case luna::graphics::VertexElementUsage::UsageTangent:
	case luna::graphics::VertexElementUsage::UsageColor:
		return 0;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture0:
		return 1;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture1:
		return 2;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture2:
		return 3;
		break;
	case luna::graphics::VertexElementUsage::UsageTexture3:
		return 4;
		break;
	default:
		return 0;
		break;
	}
}

DXGI_FORMAT DX12PipelineStateGraphic::GetDx12ElementFormat(VertexElementType elementType, uint8_t elementCount)
{
	switch (elementType)
	{
	case luna::graphics::VertexElementType::Float:
	{
		switch (elementCount)
		{
		case 1:
			return DXGI_FORMAT::DXGI_FORMAT_R32_FLOAT;
			break;
		case 2:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT;
			break;
		case 3:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case 4:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		default:
			break;
		}
	}
	break;
	case luna::graphics::VertexElementType::Int:
	{
		switch (elementCount)
		{
		case 1:
			return DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
			break;
		case 2:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32_UINT;
			break;
		case 3:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32_UINT;
			break;
		case 4:
			return DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_UINT;
			break;
		default:
			break;
		}
	}
	break;
	default:
		return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
		break;
	}
	return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
}

void DX12PipelineStateGraphic::GetDx12InputElementDesc(const RHIVertexLayout& rhi_layout, LArray<D3D12_INPUT_ELEMENT_DESC>& input_desc_array)
{
	input_desc_array.resize(rhi_layout.mElements.size());
	for (int32_t element_index = 0; element_index < rhi_layout.mElements.size(); ++element_index)
	{ 
		input_desc_array[element_index].SemanticName = GetDx12ElementName(rhi_layout.mElements[element_index].mUsage);
		input_desc_array[element_index].SemanticIndex = GetDx12ElementIndex(rhi_layout.mElements[element_index].mUsage);
		input_desc_array[element_index].Format = GetDx12ElementFormat(rhi_layout.mElements[element_index].mElementType, rhi_layout.mElements[element_index].mElementCount);
		if(rhi_layout.mElements[element_index].mInstanceUsage == VertexElementInstanceType::PerInstance)
		{
			input_desc_array[element_index].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
			input_desc_array[element_index].InstanceDataStepRate = 1;
		}
		else
		{
			input_desc_array[element_index].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			input_desc_array[element_index].InstanceDataStepRate = 0;
		}
		input_desc_array[element_index].InputSlot = rhi_layout.mElements[element_index].mBufferSlot;
		input_desc_array[element_index].AlignedByteOffset = rhi_layout.mElements[element_index].mOffset;
	}
}

void DX12PipelineStateGraphic::CreateGraphDrawPipelineImpl(
	RHIPipelineStateGraphDrawDesc* graphPipelineDesc,
	RHIVertexLayout& inputLayout,
	RenderPassDesc& renderPassDesc
)
{
	ID3D12Device* dx12Device = sGlobelRenderDevice->As<DX12Device>()->GetDx12Device();
	assert(dx12Device != nullptr);
	//读取pipeline的信息
	D3D12_GRAPHICS_PIPELINE_STATE_DESC dx12Desc = GetPipelineDesc(*graphPipelineDesc, renderPassDesc);

	for (auto& eachShader : graphPipelineDesc->mShaders)
	{
		switch (eachShader.first)
		{
		case RHIShaderType::Vertex:
		{
			dx12Desc.VS = CD3DX12_SHADER_BYTECODE(eachShader.second->As<DX12ShaderBlob>()->GetDX12Blob());
			break;
		}
		case RHIShaderType::Pixel:
		{
			dx12Desc.PS = CD3DX12_SHADER_BYTECODE(eachShader.second->As<DX12ShaderBlob>()->GetDX12Blob());
			break;
		}
		case RHIShaderType::Geomtry:
		{
			dx12Desc.GS = CD3DX12_SHADER_BYTECODE(eachShader.second->As<DX12ShaderBlob>()->GetDX12Blob());
			break;
		case RHIShaderType::Hull:
		{
			dx12Desc.HS = CD3DX12_SHADER_BYTECODE(eachShader.second->As<DX12ShaderBlob>()->GetDX12Blob());
			break;
		}
		case RHIShaderType::Domin:
		{
			dx12Desc.DS = CD3DX12_SHADER_BYTECODE(eachShader.second->As<DX12ShaderBlob>()->GetDX12Blob());
			break;
		}
		}
		default:
			break;
		}
	}

	DX12BindingSetLayout* dx12BindingSetLayout = mBindingSetLayout->As<DX12BindingSetLayout>();
	dx12Desc.pRootSignature = dx12BindingSetLayout->GetRootSignature();

	//获取顶点输入格式
	LArray<D3D12_INPUT_ELEMENT_DESC> input_desc_array;
	GetDx12InputElementDesc(inputLayout, input_desc_array);

	dx12Desc.InputLayout.pInputElementDescs = input_desc_array.data();
	dx12Desc.InputLayout.NumElements = static_cast<UINT>(input_desc_array.size());
	//参数填写完毕后，开始创建图形管线
	HRESULT hr = dx12Device->CreateGraphicsPipelineState(&dx12Desc, IID_PPV_ARGS(&m_pipeline_data));
	assert(SUCCEEDED(hr));
}

DX12PipelineStateCompute::DX12PipelineStateCompute(LSharedPtr<RHIPipelineStateDescBase> psoDesc) : RHIPipelineStateCompute(psoDesc)
{

}

void DX12PipelineStateCompute::CreateComputePipelineImpl(RHIPipelineStateComputeDesc* computePipelineDesc)
{
	ID3D12Device* dx12Device = sGlobelRenderDevice->As<DX12Device>()->GetDx12Device();
	assert(dx12Device != nullptr);
	D3D12_COMPUTE_PIPELINE_STATE_DESC dx12Desc = {};
	dx12Desc.CS = CD3DX12_SHADER_BYTECODE(computePipelineDesc->mShaders[RHIShaderType::Compute]->As<DX12ShaderBlob>()->GetDX12Blob());
	DX12BindingSetLayout* dx12BindingSetLayout = mBindingSetLayout->As<DX12BindingSetLayout>();
	dx12Desc.pRootSignature = dx12BindingSetLayout->GetRootSignature();
	//参数填写完毕后，开始创建图形管线
	HRESULT hr = dx12Device->CreateComputePipelineState(&dx12Desc, IID_PPV_ARGS(&m_pipeline_data));
	assert(SUCCEEDED(hr));
}


}

