#pragma once
#ifdef FBX_LIB_INITED
#include "Lib/FbxParser/FbxImporterFunc.h"
#endif

#ifdef OBJ_LIB_INITED
#include "Lib/ObjParser/ObjImporterFunc.h"
#endif

#ifdef GLTF_LIB_INITED
#include "Lib/GltfParser/GltfImporterFunc.h"
#endif
#include"Lib/ResourceImportBinding/ResourceImportBinding.h"
#include"Lib/AssetImport/MeshAssetImport.h"
namespace luna
{
	void ImportFileToLunaResource(
		const LString& filePath,
		const LString& resSavePath,
		const LString& fileName,
		const LString& fileExtension
	)
	{
		luna::resimport::LImportScene importScene;
		bool resLoaded = false;
		if (fileExtension == ".obj" || fileExtension == ".OBJ")
		{
#ifdef OBJ_LIB_INITED
			resLoaded = true;
			luna::lobj::ImportObjToLunaResource(filePath, importScene);
			importScene.PostProcessData();
#else
			assert(false);
#endif
		}
		if (fileExtension == ".fbx" || fileExtension == ".FBX")
		{
#ifdef FBX_LIB_INITED
			luna::lfbx::ImportFbxToLunaResource(filePath, importScene);
			importScene.ConvertDataAxisAndUnit(luna::resimport::LImportAxisType::ImportAxisYupLeftHand, luna::resimport::LImportUnitType::ImportUnitMeter);
			importScene.PostProcessData();
#else
			assert(false);
#endif
		}
		if (fileExtension == ".gltf" || fileExtension == ".GLTF")
		{
#ifdef GLTF_LIB_INITED
			luna::lgltf::ImportGltfToLunaResource(filePath, importScene);
			importScene.PostProcessData();
#else
			assert(false);
#endif
		}

		luna::assetimport::LMeshAssetImport meshAssetImporter;
		luna::assetimport::LAssetPack assetPackData;
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