#pragma once
#include "AssetImport/FbxParser/FbxLoader/FbxLoaderCommon.h"
namespace luna::lfbx
{
	enum class LFbxAnimationCurveType
	{
		NODE_TRANSLATION = 0,
		NODE_ROTATION,
		NODE_SCALING,
		CAMERA_FOV
	};
	
	class LFbxCurveBase
	{
		LString mNodeName;
		LFbxAnimationCurveType mCurveType;
		LArray<fbxsdk::FbxTime> mTimes;
	public:
		LFbxCurveBase(const LString& nodeName,
			LFbxAnimationCurveType curveType
		) :mNodeName(nodeName), mCurveType(curveType)
		{
		}

		inline const LString& GetName() const { return mNodeName; };

		inline const LFbxAnimationCurveType GetType() const { return mCurveType; };

		inline void AddTimePoint(FbxTime time) { mTimes.push_back(time); };

		const LArray<FbxTime>& GetTimes() const { return mTimes; };
	};

	template<typename CurveValueType>
	class LFbxTemplateCurve : public LFbxCurveBase
	{
		LArray<CurveValueType> mValue;
	public:
		LFbxTemplateCurve(const LString& nodeName,
			LFbxAnimationCurveType curveType) :LFbxCurveBase(nodeName, curveType)
		{
		}

		void AddValue(CurveValueType value)
		{
			mValue.push_back(value);
		}

		const LArray<CurveValueType>& GetValues() const { return mValue; }
	};

	class LFbxAnimationStack
	{
		LString mAnimationName;

		LArray<std::shared_ptr<LFbxCurveBase>> mAllCurves;

		fbxsdk::FbxTime mAnimStartTime;

		fbxsdk::FbxTime mAnimEndTime;

		int32_t mFramePerSecond;
	public:
		LFbxAnimationStack(const LString& name) :mAnimationName(name) {};

		void AddCurve(std::shared_ptr<LFbxCurveBase> newCurve) { mAllCurves.push_back(newCurve); };

		const LArray<std::shared_ptr<LFbxCurveBase>>& GetCurves() const { return mAllCurves; }

		void SetAnimMessage(
			fbxsdk::FbxTime animStartTime,
			fbxsdk::FbxTime animEndTime,
			int32_t framePerSecond
		)
		{
			mAnimStartTime = animStartTime;
			mAnimEndTime = animEndTime;
			mFramePerSecond = framePerSecond;
		}

		fbxsdk::FbxTime GetTimeStart() const { return mAnimStartTime; }

		fbxsdk::FbxTime GetTimeEnd() const { return mAnimEndTime; }

		int32_t GetFramePerSecond() const { return mFramePerSecond; }
	};

	class LFbxAnimationLoader
	{
	public:
		LFbxAnimationLoader() {};
		void ParsingData(const fbxsdk::FbxAnimStack* curAnimStack, LArray<LFbxAnimationStack>& outAnim, LFbxLoadContext context);
	private:
		FbxTimeSpan GetAnimationTimeSpan(const LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut, const fbxsdk::FbxAnimStack* AnimStack);

		void FindAllAnimateNode(fbxsdk::FbxNode* rootNode, const fbxsdk::FbxAnimStack* curAnimStack,LUnorderedMap<LString,fbxsdk::FbxNode*> &allNodeOut);

		bool CheckNodeHasTransformAnimation(fbxsdk::FbxNode* targetNode, FbxAnimLayer* animLayer);

		void GetNodeFromAnimLayer(fbxsdk::FbxNode* parentNode, FbxAnimLayer* animLayer, LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut);

		void SampleKeyValue(
			LArray<fbxsdk::FbxAnimCurve*> newCurves,
			LFbxTemplateCurve<FbxDouble3>* curveOut,
			FbxTime startTime,
			FbxTime endTime,
			FbxTime lerpDelta
		);

		void ChangeAnimationLength(
			fbxsdk::FbxAnimCurve* curvePointer,
			fbxsdk::FbxTime& timeStart,
			fbxsdk::FbxTime& timeEnd
		);

		void CheckAnimationLength(
			fbxsdk::FbxAnimLayer* animLayer,
			LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut,
			fbxsdk::FbxTime& timeStart,
			fbxsdk::FbxTime& timeEnd
		);

		void GenerateConstantKeyCurve(FbxDouble3 constantValue,LFbxTemplateCurve<FbxDouble3>* curveOut, FbxTime startTime, FbxTime endTime, FbxTime lerpDelta);

		void ResampleFloat3CurveValue(
			FbxDouble3 defaultValue,
			LArray<fbxsdk::FbxAnimCurve*> &curvesArray,
			LFbxTemplateCurve<FbxDouble3>* newCurveData,
			FbxTime startTime,
			FbxTime endTime,
			FbxTime lerpDelta
		);

		void AddFloat3CurveToStack(
			FbxPropertyT<FbxDouble3> &curvesProperty,
			fbxsdk::FbxAnimLayer* baseAnimLayer,
			const LString &nodeName,
			LFbxAnimationCurveType curveType,
			FbxTime startTime,
			FbxTime endTime,
			FbxTime lerpDelta,
			LFbxAnimationStack& animStack
		);
	};

}