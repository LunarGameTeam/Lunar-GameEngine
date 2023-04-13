#pragma once
#include "AssetImport/FbxParser/FbxImporter/FbxImporterCommon.h"
#include "AssetImport/FbxParser/FbxLoader/FbxSkeletonLoader.h"
namespace luna::lfbx
{
	class LFbxContextComponentSkeleton : public LFbxContextComponent
	{
		LUnorderedMap<LString, size_t> mExistSkeletonMap;
	public:
		void Insert(const LString& skeletonHash, size_t skeletonDataId);
		
		bool Check(const LString& skeletonHash);

		size_t Get(const LString& skeletonHash);
	};

	class LFbxImporterSkeleton : public LFbxImporterBase
	{
	public:
		LFbxImporterSkeleton() {};
	private:
		
		void ParsingDataImpl(
			const size_t nodeIdex,
			const LFbxDataBase* fbxDataInput,
			const LFbxNodeBase& fbxNodeInput,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		) override;
		
		void GetFbxSkeletonHash(const LFbxDataSkeleton* skeletonData, LString& skeletonHash);
	};
}