#pragma once
#include "AssetImport/GltfParser/GltfImporter/GltfImporterCommon.h"
namespace luna::asset
{

class LGltfDataMesh;

class LGltfContextComponentMesh : public LGltfContextComponent
{
	LUnorderedMap<LString, size_t> mExistMeshMap;
public:
	LGltfContextComponentMesh() : LGltfContextComponent(asset::LImportNodeDataType::ImportDataMesh)
	{

	};

	void Insert(const LString& meshHash, size_t meshDataId);

	bool Check(const LString& meshHash);

	size_t Get(const LString& meshHash);
};

class ASSET_IMPORT_API LGltfImporterMesh : public LGltfImporterBase
{
public:
	LGltfImporterMesh() {};
private:
	void ParsingDataImpl(
		const LGltfDataBase* gltfDataInput,
		const LArray<size_t>& gltfNodesInput,
		LGltfImportContext& dataContext,
		LImportScene& outputScene
	) override;

	void AppendVector2ToString(const LVector2f& vecData, LString& hashString);

	void AppendVector3ToString(const LVector3f& vecData, LString& hashString);

	void AppendVector4ToString(const LVector4f& vecData, LString& hashString);

	LString GenerateMeshHash(const LGltfDataMesh* gltfMeshData);

};

}