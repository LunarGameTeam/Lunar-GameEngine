#pragma once
#include "lib/fbx_helper/fbx_importer_func.h"
#include "lib/fbx_helper/fbx_importer.h"
namespace luna
{
	void ImportFbxToLunaMesh(const LString& fbx_file_path, render::MeshAsset* mesh_out)
	{
		LFbxImporterHelper importer;
		LFbxSceneData scene_out;
		importer.LoadFbxFile(fbx_file_path, scene_out);
		LFbxMeshAssetHelper asset_builder;
		asset_builder.ExchangeFbxSceneToLunaMesh(scene_out, mesh_out);
	}
}