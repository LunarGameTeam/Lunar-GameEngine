#include"LBasicTexture.h"
luna::LBasicTexture::LBasicTexture(const luna::LString& file_name, LSharedObject* parent):m_root_signature_data(parent)
{
}
luna::LResult luna::LBasicTexture::Create()
{
	luna::LResult check_error;
	Json::Value new_value_root_signature;
	check_error = PancyJsonTool::GetInstance()->LoadJsonFile(m_file_name.c_str(), new_value_root_signature);
	if (!check_error.m_IsOK)
	{
		return check_error;
	}
	auto resource_data = luna::ILunarGraphicDeviceCore::GetInstance()->CreateTextureResourceFromFile(m_file_name, new_value_root_signature, true);
	if (resource_data == nullptr)
	{
		return luna::g_Failed;
	}
	m_root_signature_data = resource_data;
	return luna::g_Succeed;
}