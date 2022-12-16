#pragma once

#include "core/memory/ptr.h"

#include "render/render_config.h"
#include "render/renderer/render_target.h"
#include "render/asset/mesh_asset.h"
#include "render/renderer/material.h"


namespace luna::render
{
struct RENDER_API RenderObject
{
	SubMesh* mMesh;
	MaterialInstance* mMaterial;
	LMatrix4f* mWorldMat;
	
	bool mCastShadow = true;
	bool mReceiveLight = true;
	bool mReceiveShadow = true;

	uint64_t mID;

	RenderObject();
};
}
