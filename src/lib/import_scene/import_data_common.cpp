#define RESOURCE_IMPORT_EXPORT
#include"import_data_common.h"
namespace luna::resimport
{
	LImportNodeDataBase::LImportNodeDataBase(const LImportNodeDataType type, const size_t index) :mType(type), mIndex(index)
	{
	};

	void LImportNodeDataBase::ConvertDataAxisAndUnit(LMatrix4f convertInvMatrix, LMatrix4f convertMatrix)
	{
		ConvertDataAxisAndUnitImpl(convertInvMatrix, convertMatrix);
	}
}

