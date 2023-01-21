#pragma once
#include "core/math/math.h"
#include "core/foundation/container.h"
#include "core/foundation/string.h"
#ifndef RESOURCE_IMPORT_API
#ifdef RESOURCE_IMPORT_EXPORT
#define RESOURCE_IMPORT_API __declspec( dllexport )//∫Í∂®“Â
#else
#define RESOURCE_IMPORT_API __declspec( dllimport )
#endif
#endif
namespace luna::resimport
{
	enum class LImportNodeDataType : std::uint8_t
	{
		ImportDataMesh = 0,
		ImportDataCamera,
		ImportDataMaterial
	};
	enum LImportAxisType : std::uint8_t
	{
		ImportAxisZupLeftHand = 0,
		ImportAxisYupLeftHand,
		ImportAxisZupRightHand,
		ImportAxisYupRightHand
	};
	enum LImportUnitType : std::uint8_t
	{
		ImportUnitMeter = 0,
		ImportUnitDeciMeter,
		ImportUnitCenterMeter,
		ImportUnitInch
	};
	struct RESOURCE_IMPORT_API LImportSceneNode
	{
		size_t mIndex;
		size_t mParent;
		LString mName;
		LVector3f mTranslation;
		LQuaternion mRotation;
		LVector3f mScal;
		LMatrix4f mExtrMatrix;
		std::unordered_map<LImportNodeDataType, size_t> mNodeData;
		std::vector<size_t> mChild;
		LImportSceneNode();
	};
	class RESOURCE_IMPORT_API LImportNodeDataBase
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

