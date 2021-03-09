#include "mesh.h"
#include "legacy_render/render_subsystem.h"
#include "core/core_module.h"
#include "core/subsystem/sub_system.h"
#include <d3dcompiler.h>
#include <D3DCompiler.inl>


#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace luna
{
namespace legacy_render
{

bool Mesh::Init()
{
	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	static auto *device = render->GetDevice()->GetD3DDevice();
	static auto *context = render->GetDevice()->GetD3DDeviceContext();

	for (SubMesh &submesh : m_sub_meshes)
	{
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;
		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VERTEX) * (UINT)submesh.vertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = submesh.vertices.data();
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &submesh.vertex_buffer);
		if (FAILED(result))
		{
			return false;
		}

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * (UINT)submesh.indices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = submesh.indices.data();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &submesh.index_buffer);
		if (FAILED(result))
		{
			return false;
		}


	}
	
	return true;
}

void Mesh::Bind()
{
	for (SubMesh &submesh : m_sub_meshes)
	{
		submesh.Bind();
	}
}

void Mesh::Draw()
{
	for (SubMesh &submesh : m_sub_meshes)
	{
		submesh.Draw();
	}
}

const LMatrix4f &Mesh::GetWolrdMatrix()
{
	LTransform tmp = LTransform::Identity();
	tmp.translate(m_pos);
	tmp.rotate(m_rotation);
	tmp.scale(m_scale);
	m_cache_matrix = tmp.matrix();
	return m_cache_matrix;
}

SubMesh ProcessMesh(aiMesh *node, const aiScene *scene);
void ProcessNode(LVector<SubMesh> &submesh, aiNode *node, const aiScene *scene);

void ProcessNode(LVector<SubMesh>& submesh ,aiNode *node, const aiScene *scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		submesh.push_back(ProcessMesh(mesh, scene));
		
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		ProcessNode(submesh, node->mChildren[i], scene);
	}
}

SubMesh ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	SubMesh sub_mesh;
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		VERTEX vertex;

		vertex.pos[0] = mesh->mVertices[i].x;
		vertex.pos[1] = mesh->mVertices[i].y;
		vertex.pos[2] = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.uv[0] = (float)mesh->mTextureCoords[0][i].x;
			vertex.uv[1] = (float)mesh->mTextureCoords[0][i].y;
		}

		sub_mesh.vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			sub_mesh.indices.push_back(face.mIndices[j]);
	}
	return sub_mesh;
}

void Mesh::OnAssetFileLoad(LSharedPtr<AssetMetaData> meta, LSharedPtr<LFile> file)
{
	Assimp::Importer importer;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.
	auto length = file->GetData().size();
	const aiScene *scene = importer.ReadFileFromMemory(file->GetData().data(),length,
											 aiProcess_CalcTangentSpace |
											 aiProcess_Triangulate |
											 aiProcess_JoinIdenticalVertices |
											 aiProcess_SortByPType);
	aiNode *node = scene->mRootNode;
	ProcessNode(m_sub_meshes, node, scene);
	Init();
	return;
}

void SubMesh::Bind()
{
	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	static auto *device = render->GetDevice()->GetD3DDevice();
	static auto *context = render->GetDevice()->GetD3DDeviceContext();
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VERTEX);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SubMesh::Draw()
{
	static RenderSubusystem *render = gEngine->GetSubsystem<RenderSubusystem>();
	static auto *device = render->GetDevice()->GetD3DDevice();
	static auto *context = render->GetDevice()->GetD3DDeviceContext();
	context->DrawIndexed(indices.size(), 0, (UINT)0);
}

}
}