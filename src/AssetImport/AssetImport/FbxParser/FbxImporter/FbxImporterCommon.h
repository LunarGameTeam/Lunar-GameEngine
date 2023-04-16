#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxLoaderUtil.h"
#include "AssetImport/ImportScene/ImportScene.h"
#include "AssetImport/FbxParser/FbxLoader/FbxSceneLoader.h"
namespace luna::lfbx
{
	void FbxEulaToQuaternion(const fbxsdk::FbxDouble3& in, LQuaternion& out);

	void FbxVector3ToVector3(const fbxsdk::FbxDouble3& in, LVector3f& out);

	void FbxVector4ToVector3(const fbxsdk::FbxDouble4& in, LVector3f& out);

	void FbxVector4ToVector4(const fbxsdk::FbxDouble4& in, LVector4f& out);

	void FbxColorToVector4(const fbxsdk::FbxColor& in, LVector4f& out);

	void FbxVector2ToVector2(const fbxsdk::FbxDouble2& in, LVector2f& out);

	void FbxMatrixToMatrix(const fbxsdk::FbxAMatrix& in, LMatrix4f& out);
	
	class LFbxContextComponent
	{
		asset::LImportNodeDataType mType;
	public:
		
		LFbxContextComponent(asset::LImportNodeDataType type) :mType(type) {}
		
		asset::LImportNodeDataType GetType() { return mType; }
	};

	class LFbxImportContext
	{
		LUnorderedMap<asset::LImportNodeDataType, LSharedPtr<LFbxContextComponent>> mComponents;
	public:

		LFbxContextComponent* GetComponent(asset::LImportNodeDataType compType);

		void AddComponent(LSharedPtr<LFbxContextComponent> compData);
	};

	class LFbxImporterBase
	{
	public:
		LFbxImporterBase() {};
		void ParsingData(
			const size_t nodeIdex,
			const LFbxDataBase* fbxDataInput,
			const LFbxSceneData* fbxScene,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		);
	private:
		virtual void ParsingDataImpl(
			const size_t nodeIdex,
			const LFbxDataBase* fbxDataInput,
			const LFbxSceneData* fbxScene,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		) = 0;
	};

	class LFbxAnimationImporterBase
	{
	public:
		LFbxAnimationImporterBase() {};
		void ParsingAnimation(
			const LFbxAnimationStack* fbxAnimationInput,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		);
	private:
		virtual void ParsingAnimationImpl(
			const LFbxAnimationStack* fbxDataInput,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		) = 0;
	};
}