#include"LBasicPipeline.h"
#pragma comment ( lib, "D3DCompiler.lib")
using namespace luna;
LRootSignature::LRootSignature(const luna::LString& file_name, LSharedObject* parent) :m_root_signature_data(parent)
{
	m_file_name = file_name;
}
luna::LResult LRootSignature::Create()
{
	luna::LResult check_error;
	Json::Value new_value_root_signature;
	check_error = PancyJsonTool::GetInstance()->LoadJsonFile(m_file_name.c_str(), new_value_root_signature);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	auto resource_data = luna::ILunarGraphicDeviceCore::GetInstance()->CreateRootSignatureFromFile(m_file_name, new_value_root_signature, true);
	if (resource_data == nullptr)
	{
		return luna::g_Failed;
	}
	m_root_signature_data = resource_data;
	return luna::g_Succeed;
}


LPipelineStateObject::LPipelineStateObject(const luna::LString& file_name, LSharedObject* parent) :m_pipeline_state_data(parent)
{
	m_file_name = file_name;
}
luna::LResult LPipelineStateObject::Create()
{
	luna::LResult check_error;
	Json::Value new_value_pipeline_state;
	check_error = PancyJsonTool::GetInstance()->LoadJsonFile(m_file_name.c_str(), new_value_pipeline_state);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	auto resource_data = ILunarGraphicDeviceCore::GetInstance()->CreateRootSignatureFromFile(m_file_name, new_value_pipeline_state, true);
	if (resource_data == nullptr)
	{
		return luna::g_Failed;
	}
	m_pipeline_state_data = resource_data;
	return luna::g_Succeed;
}
