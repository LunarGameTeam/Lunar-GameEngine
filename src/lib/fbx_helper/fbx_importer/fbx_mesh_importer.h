#pragma once
#include "fbx_importer_common.h"
namespace luna::lfbx
{
	class LFbxImporterMesh : public LFbxImporterBase
	{
	public:
		LFbxImporterMesh() {};
	private:
		void ParsingDataImpl(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene) override;
	};
}