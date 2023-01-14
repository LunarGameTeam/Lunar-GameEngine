#pragma once
#include "../obj_loader/obj_loader_util.h"
#include "../../import_scene/import_scene.h"
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