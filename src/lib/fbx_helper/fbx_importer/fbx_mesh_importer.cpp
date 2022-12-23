#pragma once
#include "fbx_mesh_importer.h"
#include "../fbx_loader/fbx_mesh_loader.h"
namespace luna::lfbx
{
	void LFbxImporterMesh::ParsingDataImpl(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene)
	{
		const LFbxDataMesh* meshData = static_cast<const LFbxDataMesh*>(fbxDataInput);
		int a = 0;
	}
}