#pragma once
#include "fbx_importer_common.h"
namespace luna::lfbx
{
	void FbxEulaToQuaternion(const fbxsdk::FbxDouble3& in, LQuaternion& out)
	{
		out = Eigen::AngleAxisf(in.mData[0], Eigen::Vector3f::UnitX())
			* Eigen::AngleAxisf(in.mData[1], Eigen::Vector3f::UnitY())
			* Eigen::AngleAxisf(in.mData[2], Eigen::Vector3f::UnitZ());
	}

	void FbxVector3ToVector3(const fbxsdk::FbxDouble3& in, LVector3f& out)
	{
		out.x() = in.mData[0];
		out.y() = in.mData[1];
		out.z() = in.mData[2];
	}

	void FbxVector4ToVector3(const fbxsdk::FbxDouble4& in, LVector3f& out)
	{
		out.x() = in.mData[0];
		out.y() = in.mData[1];
		out.z() = in.mData[2];
	}

	void FbxVector4ToVector4(const fbxsdk::FbxDouble4& in, LVector4f& out)
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

	void FbxVector2ToVector2(const fbxsdk::FbxDouble2& in, LVector2f& out)
	{
		out.x() = in.mData[0];
		out.y() = in.mData[1];
	}

	void LFbxImporterBase::ParsingData(const LFbxDataBase* fbxDataInput, const LFbxNodeBase& fbxNodeInput, ImportData::LImportScene& outputScene)
	{
		ParsingDataImpl(fbxDataInput, fbxNodeInput,outputScene);
	}
}