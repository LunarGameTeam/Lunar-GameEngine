#pragma once
#include "ObjLoader/ObjLoaderUtil.h"
#include "Lib/ImportScene/ImportScene.h"
namespace luna::lobj
{
	class LObjImporterBase
	{
	public:
		LObjImporterBase() {};
		void ParsingData(const LObjDataBase* fbxDataInput, resimport::LImportScene& outputScene);
	private:
		virtual void ParsingDataImpl(const LObjDataBase* fbxDataInput, resimport::LImportScene& outputScene) = 0;
	};
}