#pragma once
#include "GltfLoader/GltfLoaderUtil.h"
#include "Lib/ImportScene/ImportScene.h"
namespace luna::lgltf
{
	LVector2f GetVector2FromGltfVector2(Microsoft::glTF::Vector2 dataInput);

	LVector3f GetVector3FromGltfVector3(Microsoft::glTF::Vector3 dataInput);

	LVector4f GetVector4FromGltfVector3(Microsoft::glTF::Vector3 dataInput, float wValue);

	LQuaternion GetQuaternionFromGltfQuaternion(Microsoft::glTF::Quaternion dataInput);

	LMatrix4f GetMatrix4FromGltfMatrix4(Microsoft::glTF::Matrix4 dataInput);

	class LGltfImporterBase
	{
	public:
		LGltfImporterBase() {};
		void ParsingData(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, resimport::LImportScene& outputScene);
	private:
		virtual void ParsingDataImpl(const LGltfDataBase* gltfDataInput, const LGltfNodeBase& gltfNodeInput, resimport::LImportScene& outputScene) = 0;
	};
}