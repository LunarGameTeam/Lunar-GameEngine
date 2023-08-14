#pragma once

#ifndef ASSET_IMPORT_API
#ifdef ASSET_IMPORT_EXPORT
#define ASSET_IMPORT_API __declspec( dllexport )//�궨��
#else
#define ASSET_IMPORT_API __declspec( dllimport )
#endif
#endif

#include"AssetImport/ImportScene/ImportScene.h"
#include"Core/Asset/Asset.h"
#include"Core/Asset/AssetModule.h"
namespace luna::asset
{
	struct NodeMessageAsset
	{
		LString mNodeName;
		LString mMeshAssetName;
		LString mMatAssetName;
		LVector3f mTranslation;
		LQuaternion mRotation;
		LVector3f mScale;
	};

	class LSceneAssetNodeMessage : public LObject
	{
		RegisterTypeEmbedd(LSceneAssetNodeMessage, LObject)
			LArray<NodeMessageAsset> mValue;
	public:
		void AddNodeToAsset(
			const LString& nodeName,
			const LString& meshAssetName,
			const LString& matAssetName,
			const LVector3f& translation,
			const LQuaternion& rotation,
			const LVector3f& scale
		)
		{
			NodeMessageAsset newAssetNode{
				nodeName,
				meshAssetName,
				matAssetName,
				translation,
				rotation,
				scale
			};
			mValue.push_back(newAssetNode);
		}

		size_t GetAssetNodeCount() { return mValue.size(); };

		LString GetNodeName(size_t index) { return mValue[index].mNodeName; }

		LString GetNodeMeshAssetName(size_t index) { return mValue[index].mMeshAssetName; }

		LString GetNodeMatAssetName(size_t index) { return mValue[index].mMatAssetName; }

		LVector3f GetNodeTranslation(size_t index) { return mValue[index].mTranslation; }

		LQuaternion GetNodeRotation(size_t index) { return mValue[index].mRotation; }

		LVector3f GetNodeScale(size_t index) { return mValue[index].mScale; }
	};

	class ASSET_IMPORT_API LAssetPack
	{
		LArray<Asset*> mAssetDatas;
	public:
		template<typename assetType>
		assetType* CreateAsset(LString assetName)
		{
			luna::LType* createType = LType::Get<assetType>();
			Asset* asset = createType->NewInstance<Asset>();
			asset->SetObjectName(assetName);
			mAssetDatas.push_back(asset);
			assetType* assetPtr = dynamic_cast<assetType*>(asset);
			return assetPtr;
		}

		void SerializeAllAsset(const LPath& path);

		void SerializeAllAsset(const LPath& path,const LString & assetName);

		void SerializeAllAssetToDemoScene(const LPath& path);

		Asset* GetAsset(size_t assetId);
	};

	class ASSET_IMPORT_API LAssetImportBase
	{
	public:
		void ParsingImportScene(
			const LString& resSavePath,
			const asset::LImportScene &importSceneData,
			LAssetPack& outAssetPack,
			asset::LSceneAssetNodeMessage* nodeMessage = nullptr
		);

	private:
		virtual void ParsingImportSceneImpl(
			const LString& resSavePath,
			const asset::LImportScene& importSceneData,
			LAssetPack& outAssetPack,
			asset::LSceneAssetNodeMessage* nodeMessage = nullptr
		) = 0;
	};
}

