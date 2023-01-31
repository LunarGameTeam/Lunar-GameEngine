#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxLoaderUtil.h"
#include "AssetImport/ImportScene/ImportScene.h"

namespace luna::lfbx
{
	void FbxEulaToQuaternion(const fbxsdk::FbxDouble3& in, LQuaternion& out);

	void FbxVector3ToVector3(const fbxsdk::FbxDouble3& in, LVector3f& out);

	void FbxVector4ToVector3(const fbxsdk::FbxDouble4& in, LVector3f& out);

	void FbxVector4ToVector4(const fbxsdk::FbxDouble4& in, LVector4f& out);

	void FbxColorToVector4(const fbxsdk::FbxColor& in, LVector4f& out);

	void FbxVector2ToVector2(const fbxsdk::FbxDouble2& in, LVector2f& out);

	class LFbxImporterBase
	{
	public:
		LFbxImporterBase() {};
		void ParsingData(const LFbxDataBase* fbxDataInput, const LFbxNodeBase& fbxNodeInput, asset::LImportScene& outputScene);
	private:
		virtual void ParsingDataImpl(const LFbxDataBase* fbxDataInput, const LFbxNodeBase& fbxNodeInput, asset::LImportScene& outputScene) = 0;
	};
}