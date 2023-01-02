#pragma once
#include "fbx_importer_common.h"
#include "../fbx_loader/fbx_scene_loader.h"
namespace luna::lfbx
{
	class LFbxSceneImportHelper
	{
		LUnorderedMap<ImportData::LImportNodeDataType, std::shared_ptr<LFbxImporterBase>> mImporters;
	public:
		LFbxSceneImportHelper();
		void ParseScene(const LFbxSceneData* fbxDataInput, ImportData::LImportScene& outputScene);
	private:
		void ParseSceneAxisAndUnit(
			fbxsdk::FbxAxisSystem axis,
			fbxsdk::FbxSystemUnit unit,
			ImportData::LImportScene& outputScene
		);

		void ParseSceneData(
			ImportData::LImportNodeDataType type,
			const LFbxDataBase* fbxDataInput,
			const LFbxNodeBase& fbxNodeInput,
			ImportData::LImportScene& outputScene
		);
		ImportData::LImportNodeDataType GetTypeByFbxType(LFbxDataType inType);
	};

	static std::shared_ptr<LFbxSceneImportHelper> instanceFbxSceneImport = std::make_shared<LFbxSceneImportHelper>();
}

