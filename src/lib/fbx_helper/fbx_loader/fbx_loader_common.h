#pragma once
#include "core/core_library.h"
#include<fbxsdk.h>
namespace luna::lfbx
{
	class LFbxImportLog
	{
		LVector<LString> mLogData;
		LFbxImportLog() {};
	public:
		static LFbxImportLog* mInstance;
		static LFbxImportLog* GetInstance()
		{
			return mInstance;
		}
		void AddLog(const LString log, ...);
	};

	struct LFbxNodeBase
	{
		size_t mParent;
		LVector<size_t> mChild;
		fbxsdk::FbxAMatrix mTransform;
		size_t mIndex;
		LString mName;
	};

	enum LFbxDataType
	{
		FbxMeshData = 0,
		FbxSkeletonData,
		FbxAnimationData,
		FbxMaterialData,
		FbxCameraData,
	};

	class LFbxDataBase
	{
		LFbxDataType mType;
	public:
		LFbxDataBase(LFbxDataType type) { mType = type; };
		LFbxDataType GetType() const { return mType; };
	};

	class LFbxLoaderBase
	{
	public:
		LFbxLoaderBase() {};
		std::shared_ptr<LFbxDataBase> ParsingData(const LVector<LFbxNodeBase>& sceneNodes,fbxsdk::FbxNode* pNode, FbxManager* pManager);
	private:
		virtual std::shared_ptr<LFbxDataBase> ParsingDataImpl(const LVector<LFbxNodeBase>& sceneNodes, fbxsdk::FbxNode* pNode, FbxManager* pManager) = 0;
	};
}