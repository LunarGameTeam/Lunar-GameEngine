#pragma once

#include "texture.h"
#include "core/math/math.h"
#include "core/misc/string.h"
#include "legacy_render/render_subsystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace luna
{
namespace legacy_render
{


void Texture::OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file)
{
	throw std::logic_error("The method or operation is not implemented.");
}

}
}
