#pragma once
#include "obj_importer_common.h"
#include "../obj_loader/obj_mesh_loader.h"
namespace luna::lobj
{
	static const size_t gVertexSizeHashNum = 1048576;
	static const size_t gVertexSizeHashNumSquare = 1099511627776;
	struct RefVertexHasher {
		size_t operator()(const tinyobj::index_t& a) const {
			return (size_t)a.vertex_index * gVertexSizeHashNumSquare + (size_t)a.normal_index * gVertexSizeHashNum + (size_t)a.texcoord_index;
		}
	};

	struct RefVertexEuqaler {
		bool operator()(const tinyobj::index_t& a, const tinyobj::index_t& b) const {
			return ((a.vertex_index == b.vertex_index) && (a.normal_index == b.normal_index) && (a.texcoord_index == b.texcoord_index));
		}
	};
	class LObjImporterMesh : public LObjImporterBase
	{
	public:
		LObjImporterMesh() {};
	private:
		void ParsingDataImpl(const LObjDataBase* objDataInput, resimport::LImportScene& outputScene) override;
		void SetAttributeById(
			const tinyobj::attrib_t& attriData,
			const tinyobj::index_t& faceIndex,
			const size_t subMeshIndex,
			resimport::LImportNodeDataMesh* nowOutData
		);
	};
}