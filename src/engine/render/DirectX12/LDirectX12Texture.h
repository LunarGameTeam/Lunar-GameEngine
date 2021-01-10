#pragma once
#include"LDirectX12Resource.h"
#include<DirectXTex/DirectXTex.h>
#include<wincodec.h>
#include<LoaderHelpers.h>
#include<DDSTextureLoader.h>
#include<WICTextureLoader.h>
//纹理数据
namespace DirectXTextureLoader
{
	uint32_t MyCountMips(uint32_t width, uint32_t height);
	inline bool CheckIfDepthStencil(DXGI_FORMAT fmt);
	inline void MyAdjustPlaneResource(
		_In_ DXGI_FORMAT fmt,
		_In_ size_t height,
		_In_ size_t slicePlane,
		_Inout_ D3D12_SUBRESOURCE_DATA& res);
	HRESULT MyFillInitData(_In_ size_t width,
		_In_ size_t height,
		_In_ size_t depth,
		_In_ size_t mipCount,
		_In_ size_t arraySize,
		_In_ size_t numberOfPlanes,
		_In_ DXGI_FORMAT format,
		_In_ size_t maxsize,
		_In_ size_t bitSize,
		_In_reads_bytes_(bitSize) const uint8_t* bitData,
		_Out_ size_t& twidth,
		_Out_ size_t& theight,
		_Out_ size_t& tdepth,
		_Out_ size_t& skipMip,
		std::vector<D3D12_SUBRESOURCE_DATA>& initData);
}
namespace luna
{
	enum TextureCompressType
	{
		TEXTURE_COMPRESS_NONE = 0,
		TEXTURE_COMPRESS_BC1,
		TEXTURE_COMPRESS_BC2,
		TEXTURE_COMPRESS_BC3,
		TEXTURE_COMPRESS_BC4,
		TEXTURE_COMPRESS_BC5,
		TEXTURE_COMPRESS_BC6,
		TEXTURE_COMPRESS_BC7
	};
	enum LunarTextureType
	{
		Texture_Static_Load = 0,
		Texture_Render_Target
	};
	struct LunarCommonTextureDesc
	{
		D3D12_HEAP_TYPE heap_type;
		D3D12_HEAP_FLAGS heap_flag_in;
		LunarTextureType texture_type;
		D3D12_RESOURCE_DESC texture_res_desc = {};
		std::string texture_data_file;
		bool if_gen_mipmap; //是否为无mipmap的纹理创建mipmap
		bool if_force_srgb; //是否强制转换为线性空间纹理
		int  max_size;      //纹理最大大小
	};
	INIT_REFLECTION_CLASS(LunarCommonTextureDesc,
		reflect_data.heap_type,
		reflect_data.heap_flag_in,
		reflect_data.texture_type,
		reflect_data.texture_res_desc.Dimension,
		reflect_data.texture_res_desc.Alignment,
		reflect_data.texture_res_desc.Width,
		reflect_data.texture_res_desc.Height,
		reflect_data.texture_res_desc.DepthOrArraySize,
		reflect_data.texture_res_desc.MipLevels,
		reflect_data.texture_res_desc.Format,
		reflect_data.texture_res_desc.SampleDesc.Count,
		reflect_data.texture_res_desc.SampleDesc.Quality,
		reflect_data.texture_res_desc.Layout,
		reflect_data.texture_res_desc.Flags,
		reflect_data.texture_data_file,
		reflect_data.if_gen_mipmap,
		reflect_data.if_force_srgb,
		reflect_data.max_size
	);
	class LDx12GraphicResourceTexture :public LTemplateAsset<LunarCommonTextureDesc>
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC  srv_desc_from_file = {};  //默认纹理访问格式
		bool if_cube_map;                                         //是否是cubemap(仅dds有效)
		size_t subresources_size = 0;
		LDirectx12ResourceBlock* texture_data = nullptr;          //buffer数据指针
	public:
		LDefaultAssert(LDx12GraphicResourceTexture, LunarCommonTextureDesc)
		~LDx12GraphicResourceTexture();
		inline ID3D12Resource* GetResource()
		{
			if (texture_data != nullptr)
			{
				return texture_data->GetResource();
			}
			return nullptr;
		}
		luna::LResult SaveTextureToFile(
			ID3D11Device* pDevice,
			const std::string& file_name,
			bool if_automip = false,
			bool if_compress = false
		);
		luna::LResult LoadResoureDataByOtherFile(const std::string& file_name);
	private:
		void CheckIfLoadingStateChanged(LLoadState& m_object_load_state) override;
		luna::LResult InitResorceByDesc(const LunarCommonTextureDesc& resource_desc) override;
	private:
		luna::LResult LoadPictureFromFile(ID3D12Device* device_data,LunarCommonTextureDesc& texture_desc);
		luna::LResult BuildEmptyPicture(ID3D12Device* device_data, const LunarCommonTextureDesc& texture_desc);
		//将纹理图片采样至windows图片中
		luna::LResult CaptureTextureDataToWindows(DirectX::ScratchImage* new_image);
		std::string GetFileTile(const std::string& data_input);
		luna::LResult BuildTextureResource(
			ID3D12Device* device_data,
			const D3D12_RESOURCE_DIMENSION& resDim,
			const size_t& width,
			const size_t& height,
			const size_t& depth,
			const size_t& mipCount,
			const size_t& arraySize,
			DXGI_FORMAT& format,
			const D3D12_RESOURCE_FLAGS& resFlags,
			const unsigned int& loadFlags,
			const int32_t& subresources_num,
			LunarCommonTextureDesc& new_texture_desc
		);
		luna::LResult UpdateTextureResource(std::vector<D3D12_SUBRESOURCE_DATA>& subresources, const LunarCommonTextureDesc& texture_desc);
		void RebuildTextureDataPath(const std::string& json_file_name, std::string& tex_data_file_name);
		void GetJsonFilePath(const std::string& json_file_name, std::string& file_path_out);
	};
	void InitTextureJsonReflect();
}