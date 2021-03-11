#pragma once

#include "core/math/math.h"
#include "core/asset/asset.h"
#include "core/misc/string.h"
#include "legacy_render/private_render.h"


namespace luna
{
namespace legacy_render
{
enum class LEGACY_RENDER_API TextureFormat
{
	FMT_Unknown,
	FMT_R8G8B8A8,
};

enum class LEGACY_RENDER_API TextureType
{
	Texture_None,
	Texture_2D,
	Texture_3D,
	Texture_Cube
};

class LEGACY_RENDER_API Texture : public LBasicAsset
{

public:
	void OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file) override;
	virtual unsigned char *GetData() = 0;
	virtual size_t GetDataSize() = 0;	

	GET_SET_VALUE(TextureFormat, m_fomat, TextureDataFormat);
	GET_SET_VALUE(TextureType, m_fomat, TextureType);

	GETTER(unsigned int, m_width, Width);
	GETTER(unsigned int, m_height, Height);

private:
	TextureFormat m_fomat;
	TextureType m_type;
	ID3D11Texture2D *m_texture;
	unsigned int m_width;
	unsigned int m_height;

};

}
}
