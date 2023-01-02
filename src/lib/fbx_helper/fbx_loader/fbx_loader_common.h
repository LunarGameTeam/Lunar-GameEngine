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

	enum LFbxDataType
	{
		FbxMeshData = 0,
		FbxSkeletonData,
		FbxAnimationData,
		FbxMaterialData,
		FbxCameraData,
	};

	struct LFbxNodeBase
	{
		size_t mParent;
		LVector<size_t> mChild;
		FbxDouble3 mLocalTranslation;
		FbxDouble3 mLocalRotation;
		FbxDouble3 mLocalScaling;
		fbxsdk::FbxAMatrix mGlobelTransform;
		std::unordered_map<LFbxDataType, size_t> mNodeData;
		size_t mIndex;
		LString mName;
	};

	class LFbxDataBase
	{
		LFbxDataType mType;
		size_t mNodeIndex;
	public:
		LFbxDataBase(LFbxDataType type) { mType = type; };
		void SetNodeIndex(size_t nodeIndex) { mNodeIndex = nodeIndex; }
		size_t GetNodeIndex() const{ return mNodeIndex; };
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