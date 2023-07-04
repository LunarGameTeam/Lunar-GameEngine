#pragma once
#include "AssetImport/FbxParser/FbxImporter/FbxImporterCommon.h"
namespace luna::lfbx
{
	class LFbxImporterSkeletonAnimation : public LFbxAnimationImporterBase
	{
	public:
		LFbxImporterSkeletonAnimation() {};
	private:

		void ParsingAnimationImpl(
			const LFbxAnimationStack* fbxDataInput,
			LFbxImportContext& dataContext,
			asset::LImportScene& outputScene
		) override;

		void TryGenerateSkeletonAnimation(
			const LFbxAnimationStack* fbxDataInput,
			const LUnorderedMap<LString, LArray<size_t>> &nodeTransformCurveMark,
			const asset::LImportNodeDataSkeleton* skeletonData,
			asset::LImportScene& outputScene
		);

		void FbxEulaCurveToImportData(const LFbxTemplateCurve<FbxDouble3>* fbxCurve, asset::LImportNodeSkeletonAnimation* newAnimation, fbxsdk::FbxTime startTime);
	};
	
}