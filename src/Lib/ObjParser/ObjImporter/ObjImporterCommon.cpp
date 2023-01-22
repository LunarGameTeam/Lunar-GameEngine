#pragma once
#include "ObjImporter/ObjImporterCommon.h"
namespace luna::lobj
{
	void LObjImporterBase::ParsingData(const LObjDataBase* fbxDataInput, resimport::LImportScene& outputScene)
	{
		ParsingDataImpl(fbxDataInput,outputScene);
	}
}