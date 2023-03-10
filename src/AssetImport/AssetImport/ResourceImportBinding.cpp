#pragma once
#include "AssetImport/FbxParser/FbxImporterFunc.h"

#include "AssetImport/ObjParser/ObjImporterFunc.h"

#include "AssetImport/GltfParser/GltfImporterFunc.h"

#include"AssetImport/ResourceImportBinding.h"
#include"AssetImport/MeshAssetImport.h"

namespace luna
{
	void ImportFileToLunaResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension
	)
	{
		luna::asset::LImportScene importScene;
		bool resLoaded = false;
		if (fileExtension == ".obj" || fileExtension == ".OBJ")
		{
			resLoaded = true;
			luna::asset::ImportObjToLunaResource(filePath, importScene);
			importScene.PostProcessData();
		}
		if (fileExtension == ".fbx" || fileExtension == ".FBX")
		{
			luna::asset::ImportFbxToLunaResource(filePath, importScene);
			importScene.ConvertDataAxisAndUnit(luna::asset::LImportAxisType::ImportAxisYupLeftHand, luna::asset::LImportUnitType::ImportUnitMeter);
			importScene.PostProcessData();
		}
		if (fileExtension == ".gltf" || fileExtension == ".GLTF")
		{
			luna::asset::ImportGltfToLunaResource(filePath, importScene);
			importScene.PostProcessData();
		}

		luna::asset::LMeshAssetImport meshAssetImporter;
		luna::asset::LAssetPack assetPackData;
		meshAssetImporter.ParsingImportScene(importScene, assetPackData);
		assetPackData.SerializeAllAsset(resSavePath, fileName);
	}
	STATIC_INIT(ResourceImport)
	{
		BindingModule* resourceParseModule = BindingModule::Get("luna.resource_parse");
		resourceParseModule->AddMethod<&ImportFileToLunaResource>("import_resource_file");
		resourceParseModule->Init();
	};
}