#pragma once
#include "Core/Math/Math.h"
#include "Core/Foundation/Container.h"
#include "Core/Foundation/String.h"

#ifndef ASSET_IMPORT_API
#ifdef ASSET_IMPORT_EXPORT
#define ASSET_IMPORT_API __declspec( dllexport )
#else
#define ASSET_IMPORT_API __declspec( dllimport )
#endif
#endif

namespace luna::asset
{
	enum class LImportNodeDataType : uint8_t
	{
		ImportDataMesh = 0,
		ImportDataCamera,
		ImportDataMaterial
	};

	enum LImportAxisType : uint8_t
	{
		ImportAxisZupLeftHand = 0,
		ImportAxisYupLeftHand,
		ImportAxisZupRightHand,
		ImportAxisYupRightHand
	};

	enum LImportUnitType : uint8_t
	{
		ImportUnitMeter = 0,
		ImportUnitDeciMeter,
		ImportUnitCenterMeter,
		ImportUnitInch
	};

	struct ASSET_IMPORT_API LImportSceneNode
	{
		size_t mIndex;
		size_t mParent;
		LString mName;
		LVector3f mTranslation;
		LQuaternion mRotation;
		LVector3f mScal;
		LMatrix4f mExtrMatrix;
		std::unordered_map<LImportNodeDataType, size_t> mNodeData;
		LArray<size_t> mChild;
		LImportSceneNode();
	};

	class ASSET_IMPORT_API LImportNodeDataBase
	{
		LImportNodeDataType mType;
		size_t mIndex;
	public:
		LImportNodeDataBase(const LImportNodeDataType type, const size_t index);

		inline LImportNodeDataType GetType() { return mType; }

		void ConvertDataAxisAndUnit(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix);
	private:
		virtual void ConvertDataAxisAndUnitImpl(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix) = 0;
	};
}

