#include "shader.h"
#include "legacy_render/render_subsystem.h"
#include "core/core_module.h"
#include "core/subsystem/sub_system.h"
#include <d3dcompiler.h>
#include <D3DCompiler.inl>

namespace luna
{
namespace legacy_render
{

void OutputShaderErrorMessage(ID3D10Blob *errorMessage)
{
	char *compileErrors;
	// Get a pointer to the error message text buffer.
	compileErrors = (char *)(errorMessage->GetBufferPointer());
	// Get the length of the message.
	auto bufferSize = errorMessage->GetBufferSize();
	// Write out the error message.
	LString str(compileErrors, compileErrors + bufferSize);
	LogVerbose(E_Core, str);
	return;
}

bool Dx11Shader::Init()
{
	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	static auto *device = render->GetDevice();
	HRESULT result;
	ID3D10Blob *errorMessage;
	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(m_vs_path.GetStdUnicodeString().c_str(), NULL, NULL, "VSMain", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
								&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		OutputShaderErrorMessage(errorMessage);
		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(m_ps_path.GetStdUnicodeString().c_str(), NULL, NULL, "PSMain", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
								&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		OutputShaderErrorMessage(errorMessage);
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->GetD3DDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		OutputShaderErrorMessage(errorMessage);
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->GetD3DDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		OutputShaderErrorMessage(errorMessage);
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->GetD3DDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
													   vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	ID3D11ShaderReflection *pReflector = nullptr;
	D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void **)&pReflector);
	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.

	D3D11_SHADER_DESC ShaderDesc;
	pReflector->GetDesc(&ShaderDesc);

	for (auto i = 0; i < ShaderDesc.ConstantBuffers; i++)
	{
		ID3D11ShaderReflectionConstantBuffer *pCBReflecion = nullptr;
		pCBReflecion = pReflector->GetConstantBufferByIndex(i);

		D3D11_BUFFER_DESC matrixBufferDesc;
		D3D11_SHADER_BUFFER_DESC ShaderBufferDesc;
		pCBReflecion->GetDesc(&ShaderBufferDesc);
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = ShaderBufferDesc.Size;
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;
		m_buffer_map[ShaderBufferDesc.Name] = nullptr;
		result = device->GetD3DDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_cb0_buffer);

		if (ShaderBufferDesc.Type == D3D_CT_CBUFFER || ShaderBufferDesc.Type == D3D_CT_TBUFFER)
		{
		}
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;
	// 
	// 	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	// 	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	// 	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// 	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 	matrixBufferDesc.MiscFlags = 0;
	// 	matrixBufferDesc.StructureByteStride = 0;
	// 
	// 	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	// 	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	// 	if (FAILED(result))
	// 	{
	// 		return false;
	// 	}
	return true;

}

bool Dx11Shader::Clean()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterInt(const LString &name, int32_t value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterUInt(const LString &name, uint32_t value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterFloat(const LString &name, float value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterFloat3(const LString &name, const LVector3f &value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterFloat4(const LString &name, const LVector4f &value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterMatrix3(const LString &name, const LMatrix3f &value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Dx11Shader::SetParameterMatrix4(const LString &name, const LMatrix4f &value)
{

}


void Dx11Shader::Bind()
{
	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	static auto *context = render->GetDevice()->GetD3DDeviceContext();
	context->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	context->VSSetShader(m_vertexShader, NULL, 0);
	context->PSSetShader(m_pixelShader, NULL, 0);

}

void Dx11Shader::SetWVPMatrix(const LMatrix4f &w, const LMatrix4f &v, const LMatrix4f &p)
{

	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	static auto *context = render->GetDevice()->GetD3DDeviceContext();
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *dataPtr;
	unsigned int bufferNumber;
	
	// Lock the constant buffer so it can be written to.
	result = context->Map(m_cb0_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType *)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = w;
	dataPtr->view = v;
	DirectX::XMFLOAT4X4 mat_pers_data;
	auto mid_data = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_1DIV2PI / 2.0f, 1024.0f / 768.0f, 0.1f, 1000.0f);
	
	DirectX::XMStoreFloat4x4(&mat_pers_data, mid_data);
	dataPtr->projection = p;

	// Unlock the constant buffer.
	context->Unmap(m_cb0_buffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	context->VSSetConstantBuffers(bufferNumber, 1, &m_cb0_buffer);
}

}
}