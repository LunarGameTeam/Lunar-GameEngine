#pragma once
#ifdef FBX_LIB_INITED
#include "AssetImport/FbxParser/FbxImporterFunc.h"
#endif

#ifdef OBJ_LIB_INITED
#include "AssetImport/ObjParser/ObjImporterFunc.h"
#endif

#ifdef GLTF_LIB_INITED
#include "AssetImport/GltfParser/GltfImporterFunc.h"
#endif

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
#ifdef OBJ_LIB_INITED
			resLoaded = true;
			luna::asset::ImportObjToLunaResource(filePath, importScene);
			importScene.PostProcessData();
#else
			assert(false);
#endif
		}
		if (fileExtension == ".fbx" || fileExtension == ".FBX")
		{
#ifdef FBX_LIB_INITED
			luna::asset::ImportFbxToLunaResource(filePath, importScene);
			importScene.ConvertDataAxisAndUnit(luna::asset::LImportAxisType::ImportAxisYupLeftHand, luna::asset::LImportUnitType::ImportUnitMeter);
			importScene.PostProcessData();
#else
			assert(false);
#endif
		}
		if (fileExtension == ".gltf" || fileExtension == ".GLTF")
		{
#ifdef GLTF_LIB_INITED
			luna::asset::ImportGltfToLunaResource(filePath, importScene);
			importScene.PostProcessData();
#else
			assert(false);
#endif
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