#include "Graphics/Asset/MeshAssetUtils.h"

namespace luna::graphics
{
	void AddLineToSubMesh(const BaseVertex& v1, const BaseVertex& v2, SubMesh& submeshData)
	{
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v1);
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v2);
	}

	void AddLineToSubMesh(const LVector3f& p1, const LVector3f& p2, SubMesh& submeshData)
	{
		BaseVertex v;
		v.pos = p1;
		v.color = LVector4f(1, 1, 1, 1);
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v);
		v.pos = p2;
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v);
	}

	void AddTriangleToSubMesh(const BaseVertex& v1, const BaseVertex& v2, const BaseVertex& v3, SubMesh& submeshData)
	{
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v1);
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v2);
		submeshData.mIndexData.push_back(submeshData.mVertexData.size());
		submeshData.mVertexData.push_back(v3);
	}

	void AddCubeWiredToSubMesh(SubMesh& submeshData, const LVector3f& position, const LVector3f& size /*= LVector3f(1, 1, 1)*/, const LVector4f& color /*= LVector4f(1, 1, 1)*/)
	{
		BaseVertex v1;
		v1.color = color;
		v1.pos = position;
		float x, y, z;
		x = size.x();
		y = size.y();
		z = size.z();
		v1.pos.x() -= x * 0.5f;
		v1.pos.y() -= y * 0.5f;
		v1.pos.z() -= z * 0.5;
		BaseVertex v2 = v1;
		v2.pos.x() += x;
		AddLineToSubMesh(v1, v2, submeshData);
		BaseVertex v3 = v1;
		v3.pos.z() += z;
		AddLineToSubMesh(v1, v3, submeshData);
		BaseVertex v4 = v1;
		v4.pos.x() += x;
		v4.pos.z() += z;
		AddLineToSubMesh(v2, v4, submeshData);
		AddLineToSubMesh(v3, v4, submeshData);
		BaseVertex v5 = v1;
		BaseVertex v6 = v2;
		BaseVertex v7 = v3;
		BaseVertex v8 = v4;
		v5.pos.y() += y;
		v6.pos.y() += y;
		v7.pos.y() += y;
		v8.pos.y() += y;
		AddLineToSubMesh(v5, v6, submeshData);
		AddLineToSubMesh(v5, v7, submeshData);
		AddLineToSubMesh(v6, v8, submeshData);
		AddLineToSubMesh(v7, v8, submeshData);

		AddLineToSubMesh(v1, v5, submeshData);
		AddLineToSubMesh(v2, v6, submeshData);
		AddLineToSubMesh(v3, v7, submeshData);
		AddLineToSubMesh(v4, v8, submeshData);

	}
}
