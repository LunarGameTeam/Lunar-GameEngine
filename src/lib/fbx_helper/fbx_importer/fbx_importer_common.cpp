#pragma once
#include "fbx_importer_common.h"
namespace luna::lfbx
{
	void FbxVector4ToVector3(const fbxsdk::FbxVector4& in, LVector3f& out)
	{
		out.x() = in.mData[0];
		out.y() = in.mData[1];
		out.z() = in.mData[2];
	}

	void FbxVector4ToVector4(const fbxsdk::FbxVector4& in, LVector4f& out)
	{
		out.x() = in.mData[0];
		out.y() = in.mData[1];
		out.z() = in.mData[2];
		out.w() = in.mData[3];
	}

	void FbxColorToVector4(const fbxsdk::FbxColor& in, LVector4f& out)
	{
		out.x() = in.mRed;
		out.y() = in.mGreen;
		out.z() = in.mBlue;
		out.w() = in.mAlpha;
	}

	void FbxVector2ToVector2(const fbxsdk::FbxVector2& in, LVector2f& out)
	{
		out.x() = in.mData[0];
		out.y() = in.mData[1];
	}

	void LFbxImporterBase::ParsingData(const LFbxDataBase* fbxDataInput, ImportData::LImportScene& outputScene)
	{
		ParsingDataImpl(fbxDataInput, outputScene);
	}
}