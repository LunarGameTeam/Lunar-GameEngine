#pragma once
#include "../fbx_loader/fbx_loader_util.h"
#include "../../import_scene/import_scene.h"
namespace luna::lfbx
{
	class LFbxImporterBase
	{
	public:
		LFbxImporterBase() {};
		void ParsingData(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene);
	private:
		virtual void ParsingDataImpl(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene) = 0;
	};
}