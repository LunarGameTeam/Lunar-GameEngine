#pragma once

#include "AssetImport/ImportScene/ImportScene.h"

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/Deserialize.h>
#include <GLTFSDK/GLBResourceReader.h>
#include "AssetImport/GltfParser/GltfLoader/GltfLoaderCommon.h"

namespace luna::asset
{

LVector2f GetVector2FromGltfVector2(Microsoft::glTF::Vector2 dataInput);

LVector3f GetVector3FromGltfVector3(Microsoft::glTF::Vector3 dataInput);

LVector4f GetVector4FromGltfVector3(Microsoft::glTF::Vector3 dataInput, float wValue);

LQuaternion GetQuaternionFromGltfQuaternion(Microsoft::glTF::Quaternion dataInput);

LMatrix4f GetMatrix4FromGltfMatrix4(Microsoft::glTF::Matrix4 dataInput);

class ASSET_IMPORT_API LGltfImporterBase
{
public:
	LGltfImporterBase() {};
	void ParsingData(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, asset::LImportScene& outputScene);
private:
	virtual void ParsingDataImpl(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, asset::LImportScene& outputScene) = 0;
};

}