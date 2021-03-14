#pragma once

#include "legacy_render/asset/texture2d.h"

#include "core/core_module.h"
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

void Texture2D::OnAssetFileLoad(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	auto *device = render->GetDevice();
	int w, h, n;
	m_data = stbi_load_from_memory(file->GetData().data(), file->GetData().size(), &w, &h, &n, 4);
	m_width = w;
	m_height = h;
}

unsigned char *Texture2D::GetData()
{
	return m_data;
}

size_t Texture2D::GetDataSize()
{
	return m_height * m_width * 4;
}

}
}
