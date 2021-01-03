#pragma once
#include"ILunarGraphicCore.h"
//shader输入布局
namespace luna
{
	class LBasicTexture
	{
		luna::LString m_file_name;
		LPtrBasic m_root_signature_data;
	public:
		LBasicTexture(const luna::LString& file_name, LSharedObject* parent);
		~LBasicTexture()
		{
		};
		luna::LResult Create();
	};
}