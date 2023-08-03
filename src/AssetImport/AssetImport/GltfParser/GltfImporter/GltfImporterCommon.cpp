#pragma once
#include "AssetImport/GltfParser/GltfImporter/GltfImporterCommon.h"
namespace luna::asset
{
	LVector2f GetVector2FromGltfVector2(Microsoft::glTF::Vector2 dataInput)
	{
		LVector2f dataOutPut;
		dataOutPut.x() = dataInput.x;
		dataOutPut.y() = dataInput.y;
		return dataOutPut;
	}

	LVector3f GetVector3FromGltfVector3(Microsoft::glTF::Vector3 dataInput)
	{
		LVector3f dataOutPut;
		dataOutPut.x() = dataInput.x;
		dataOutPut.y() = dataInput.y;
		dataOutPut.z() = dataInput.z;
		return dataOutPut;
	}

	LVector4f GetVector4FromGltfVector3(Microsoft::glTF::Vector3 dataInput, float wValue)
	{
		LVector4f dataOutPut;
		dataOutPut.x() = dataInput.x;
		dataOutPut.y() = dataInput.y;
		dataOutPut.z() = dataInput.z;
		dataOutPut.w() = wValue;
		return dataOutPut;
	}

	LQuaternion GetQuaternionFromGltfQuaternion(Microsoft::glTF::Quaternion dataInput)
	{
		LQuaternion dataOutPut;
		dataOutPut.x() = dataInput.x;
		dataOutPut.y() = dataInput.y;
		dataOutPut.z() = dataInput.z;
		dataOutPut.w() = dataInput.w;
		return dataOutPut;
	}

	LMatrix4f GetMatrix4FromGltfMatrix4(Microsoft::glTF::Matrix4 dataInput)
	{
		LMatrix4f dataOutPut;
		memcpy(dataOutPut.data(), dataInput.values.data(), 16 * sizeof(float));
		return dataOutPut;
	}

	LGltfContextComponent* LGltfImportContext::GetComponent(asset::LImportNodeDataType compType)
	{
		auto value = mComponents.find(compType);
		if (value != mComponents.end())
		{
			return value->second.get();
		}
		return nullptr;
	}

	void LGltfImportContext::AddComponent(LSharedPtr<LGltfContextComponent> compData)
	{
		mComponents.insert({ compData->GetType() ,compData });
	}

	void LGltfImporterBase::ParsingData(
		const LGltfDataBase* gltfDataInput,
		const LArray<size_t>& gltfNodesInput,
		LGltfImportContext& dataContext,
		asset::LImportScene& outputScene)
	{
		ParsingDataImpl(gltfDataInput, gltfNodesInput, dataContext,outputScene);
	}
}