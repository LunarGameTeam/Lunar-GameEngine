#pragma once
#include "ObjImporter/ObjMeshImporter.h"
namespace luna::lobj
{
	void LObjImporterMesh::SetAttributeById(
		const tinyobj::attrib_t& attriData,
		const tinyobj::index_t& faceIndex,
		const size_t subMeshIndex,
		resimport::LImportNodeDataMesh* nowOutData

	)
	{
		LVector3f pos;
		LVector3f norm;
		LVector4f tangent;
		LArray<LVector2f> uv;
		LVector4f color;
		tangent.setZero();

		pos.x() = attriData.vertices[faceIndex.vertex_index * 3];
		pos.y() = attriData.vertices[faceIndex.vertex_index * 3 + 1];
		pos.z() = attriData.vertices[faceIndex.vertex_index * 3 + 2];

		norm.x() = attriData.normals[faceIndex.normal_index * 3];
		norm.y() = attriData.normals[faceIndex.normal_index * 3 + 1];
		norm.z() = attriData.normals[faceIndex.normal_index * 3 + 2];

		uv.resize(1);
		uv[0].x() = attriData.texcoords[faceIndex.texcoord_index * 2];
		uv[0].y() = attriData.texcoords[faceIndex.texcoord_index * 2 + 1];

		if (attriData.colors.size() > 0)
		{
			color.x() = attriData.colors[faceIndex.vertex_index * 3];
			color.y() = attriData.colors[faceIndex.vertex_index * 3 + 1];
			color.z() = attriData.colors[faceIndex.vertex_index * 3 + 2];
			color.w() = 1;
		}
		nowOutData->AddFullVertex(subMeshIndex,pos,norm,tangent,uv,color);
	}

	void LObjImporterMesh::ParsingDataImpl(const LObjDataBase* objDataInput, resimport::LImportScene& outputScene)
	{

		resimport::LImportSceneNode node_value;
		node_value.mIndex = outputScene.GetNodeSize();
		node_value.mName = "Node" + std::to_string(node_value.mIndex);
		node_value.mParent = -1;
		node_value.mTranslation.setIdentity();
		node_value.mRotation.setIdentity();
		node_value.mScal.setIdentity();
		node_value.mNodeData.insert(std::pair<resimport::LImportNodeDataType, size_t>(resimport::LImportNodeDataType::ImportDataMesh, node_value.mIndex));
		outputScene.AddNodeData(node_value);
		//添加mesh数据
		const LObjDataMesh* objMeshData = static_cast<const LObjDataMesh*>(objDataInput);
		resimport::LImportNodeDataMesh* nowOutData = outputScene.AddNewData<resimport::LImportNodeDataMesh>();
		//目前obj文件每个shape默认只有一个材质
		const tinyobj::attrib_t &newAttriData = objMeshData->GetAttriData();
		const std::vector<tinyobj::shape_t> &newShapesDataArray = objMeshData->GetShapesData();
		for (size_t shapeIndex = 0; shapeIndex < newShapesDataArray.size(); ++shapeIndex)
		{
			const tinyobj::shape_t& newShapeData = newShapesDataArray[shapeIndex];
			LString subMeshName = newShapeData.name;
			if (subMeshName == "")
			{
				subMeshName = "SubMesh" + std::to_string(shapeIndex);
			}
			nowOutData->AddSubMeshMessage(subMeshName, shapeIndex);
			std::unordered_map<tinyobj::index_t, int32_t, RefVertexHasher, RefVertexEuqaler> hashData;
			for (int32_t indexFace = 0; indexFace < newShapeData.mesh.indices.size(); ++indexFace)
			{
				const tinyobj::index_t& nowDealIndex = newShapeData.mesh.indices[indexFace];
				int32_t newVertexIndex = -1;
				auto existVertex = hashData.find(nowDealIndex);
				if (existVertex == hashData.end())
				{
					newVertexIndex = (int32_t)hashData.size();
					hashData.insert(std::pair<tinyobj::index_t, int32_t>(nowDealIndex, newVertexIndex));
					SetAttributeById(newAttriData, nowDealIndex, shapeIndex, nowOutData);
				}
				else
				{
					newVertexIndex = existVertex->second;
				}
				nowOutData->AddFaceIndexDataToSubmesh(shapeIndex, newVertexIndex);
			}
		}
	}
}