#pragma once
#include "fbx_importer_common.h"
#include "../fbx_loader/fbx_scene_loader.h"
namespace luna::lfbx
{
	class LFbxSceneImportHelper
	{
		LUnorderedMap<resimport::LImportNodeDataType, std::shared_ptr<LFbxImporterBase>> mImporters;
	public:
		LFbxSceneImportHelper();
		void ParseScene(const LFbxSceneData* fbxDataInput, resimport::LImportScene& outputScene);
	private:
		void ParseSceneAxisAndUnit(
			fbxsdk::FbxAxisSystem axis,
			fbxsdk::FbxSystemUnit unit,
			resimport::LImportScene& outputScene
		);

		void ParseSceneData(
			resimport::LImportNodeDataType type,
			const LFbxDataBase* fbxDataInput,
			const LFbxNodeBase& fbxNodeInput,
			resimport::LImportScene& outputScene
		);
		resimport::LImportNodeDataType GetTypeByFbxType(LFbxDataType inType);
	};

	static std::shared_ptr<LFbxSceneImportHelper> instanceFbxSceneImport = std::make_shared<LFbxSceneImportHelper>();
}

