#pragma once

#include "obj_asset.h"
#include "core/math/math.h"
#include "core/foundation/string.h"
#include "core/asset/asset_module.h"
#include "render/render_module.h"

#include "obj_loader.h"

namespace luna::render
{

RegisterTypeEmbedd_Imp(ObjAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
	cls->Ctor<ObjAsset>();
};

void ObjAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	Super::OnAssetFileRead(meta, file);
	objl::Loader loader;
	loader.LoadFile(mContent.str());
	BaseVertex v;
	
	for (auto& mesh : loader.LoadedMeshes)
	{
		mSubMesh.push_back(new SubMesh());
		SubMesh* submesh = mSubMesh.back();
		submesh->mIndexCount = mesh.Indices.size();
		submesh->mVertexCount = mesh.Vertices.size();
		submesh->mVertexData.resize(submesh->mVertexCount);
		submesh->mIndexData.resize(submesh->mIndexCount);

		for (auto idx = 0; idx < mesh.Indices.size(); idx++)
		{
			submesh->mIndexData[idx] = mesh.Indices[idx];
		}

		for (auto idx = 0 ; idx < mesh.Vertices.size(); idx++)
		{
			
			submesh->mVertexData[idx].pos.x() = mesh.Vertices[idx].Position.X;
			submesh->mVertexData[idx].pos.y() = mesh.Vertices[idx].Position.Y;
			submesh->mVertexData[idx].pos.z() = mesh.Vertices[idx].Position.Z;
			submesh->mVertexData[idx].normal.x() = mesh.Vertices[idx].Normal.X;
			submesh->mVertexData[idx].normal.y() = mesh.Vertices[idx].Normal.Y;
			submesh->mVertexData[idx].normal.z() = mesh.Vertices[idx].Normal.Z;
			submesh->mVertexData[idx].uv[0].x() = mesh.Vertices[idx].TextureCoordinate.X;
			submesh->mVertexData[idx].uv[0].y() = mesh.Vertices[idx].TextureCoordinate.Y;
			submesh->mVertexData[idx].color = LVector4f(1, 1, 1, 1);
			submesh->mVertexData[idx].tangent = LVector4f(0, 1, 0,0);			
		}
		submesh->Init();
	}
	
}

};

