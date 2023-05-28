#pragma once
#include "AssetImport/FbxParser/FbxImporter/FbxImporterCommon.h"
#include "AssetImport/FbxParser/FbxLoader/FbxSceneLoader.h"

namespace luna::lfbx
{
	class LFbxSceneImportHelper
	{
		LUnorderedMap<asset::LImportNodeDataType, std::shared_ptr<LFbxImporterBase>> mImporters;
		LUnorderedMap<asset::LImportNodeAnimationType, std::shared_ptr<LFbxAnimationImporterBase>> mAnimationImporters;
	public:
		LFbxSceneImportHelper();
		void ParseScene(const LFbxSceneData* fbxDataInput, asset::LImportScene& outputScene);
	private:
		void ParseSceneAxisAndUnit(
			fbxsdk::FbxAxisSystem axis,
			fbxsdk::FbxSystemUnit unit,
			asset::LImportScene& outputScene
		);

		void ParseSceneData(
			const asset::LImportNodeDataType type,
			const size_t nodeIdex,
			const LFbxDataBase* fbxDataInput,
			const LFbxSceneData* fbxScene,
			LFbxImportContext &dataContext,
			asset::LImportScene& outputScene
		);

		void ParseAnimationData(
			const LFbxAnimationStack* fbxAnimationInput,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		);

		asset::LImportNodeDataType GetTypeByFbxType(LFbxDataType inType);
	};

	static std::shared_ptr<LFbxSceneImportHelper> instanceFbxSceneImport = std::make_shared<LFbxSceneImportHelper>();
}

