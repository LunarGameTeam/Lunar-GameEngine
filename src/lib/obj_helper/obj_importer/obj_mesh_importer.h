#pragma once
#include "obj_importer_common.h"
#include "../obj_loader/obj_mesh_loader.h"
namespace luna::lobj
{
	class LObjImporterMesh : public LObjImporterBase
	{
	public:
		LObjImporterMesh() {};
	private:
		void ParsingDataImpl(const LObjDataBase* fbxDataInput, resimport::LImportScene& outputScene) override;
	};
}