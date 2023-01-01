#pragma once
#include "../fbx_loader/fbx_loader_util.h"
#include "../../import_scene/import_scene.h"
namespace luna::lfbx
{
	void FbxVector4ToVector3(const fbxsdk::FbxVector4& in, LVector3f& out);

	void FbxVector4ToVector4(const fbxsdk::FbxVector4& in, LVector4f& out);

	void FbxColorToVector4(const fbxsdk::FbxColor& in, LVector4f& out);

	void FbxVector2ToVector2(const fbxsdk::FbxVector2& in, LVector2f& out);

	class LFbxImporterBase
	{
	public:
		LFbxImporterBase() {};
		void ParsingData(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene);
	private:
		virtual void ParsingDataImpl(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene) = 0;
	};
}