#pragma once
#include "fbx_importer_common.h"
namespace luna::lfbx
{
	void LFbxImporterBase::ParsingData(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene)
	{
		ParsingDataImpl(fbxDataInput, outputScene);
	}
}