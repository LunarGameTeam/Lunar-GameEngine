#pragma once
#include"ILunarGraphicCore.h"
namespace luna
{
	class LBasicBuffer
	{
		luna::LString m_file_name;
		LPtrBasic m_root_signature_data;
	public:
		LBasicBuffer(const luna::LString& file_name, LSharedObject* parent);
		~LBasicBuffer();
		luna::LResult Create();
	};
}