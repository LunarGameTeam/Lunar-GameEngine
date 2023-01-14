#pragma once
#include "obj_importer_common.h"
namespace luna::lobj
{
	void LObjImporterBase::ParsingData(const LObjDataBase* fbxDataInput, resimport::LImportScene& outputScene)
	{
		ParsingDataImpl(fbxDataInput,outputScene);
	}
}