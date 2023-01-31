#pragma once
#include "AssetImport/ObjParser/ObjImporter/ObjImporterCommon.h"
namespace luna::asset
{
	void LObjImporterBase::ParsingData(const LObjDataBase* fbxDataInput, asset::LImportScene& outputScene)
	{
		ParsingDataImpl(fbxDataInput,outputScene);
	}
}