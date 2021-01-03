#pragma once
//临时的dx11头文件(纹理压缩)
#include<d3d11.h>
#include<d3d12.h>
#include<DirectXMath.h>
//#include<d3d12shader.h>
#include<d3dcompiler.h>
#include"d3dx12.h"
#include<DXGI1_6.h>
#include <wrl/client.h>
#include<comdef.h>
#include "render/RenderInterface/ILunarGraphicCore.h"
namespace luna
{
	using LDirectXGIFactory = IDXGIFactory4;
	LResult GetDirectXCommondlistType(const LunarGraphicPipeLineType& type_input, D3D12_COMMAND_LIST_TYPE& type_output);
}
