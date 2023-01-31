#pragma once

#include "AssetImport/ImportScene/ImportScene.h"

namespace luna::asset
{
class LObjDataBase;

class LObjImporterBase
{
public:
	LObjImporterBase() {};
	void ParsingData(const LObjDataBase* fbxDataInput, asset::LImportScene& outputScene);
private:
	virtual void ParsingDataImpl(const LObjDataBase* fbxDataInput, asset::LImportScene& outputScene) = 0;
};
}