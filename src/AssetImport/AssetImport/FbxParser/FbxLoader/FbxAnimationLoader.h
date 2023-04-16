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
	public:
		LFbxCurveBase(const LString& nodeName,
			LFbxAnimationCurveType curveType
		) :mNodeName(nodeName), mCurveType(curveType)
		{
		}
		inline const LString& GetName() const { return mNodeName; };
		inline const LFbxAnimationCurveType GetType() const { return mCurveType; };
	};

	template<typename CurveValueType>
	struct LFbxTemplateCurve : public LFbxCurveBase
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
	public:
		LFbxAnimationStack(const LString& name) :mAnimationName(name) {};

		void AddCurve(std::shared_ptr<LFbxCurveBase> newCurve) { mAllCurves.push_back(newCurve); };

		const LArray<std::shared_ptr<LFbxCurveBase>>& GetCurves() const { return mAllCurves; }
	};

	class LFbxAnimationLoader
	{
	public:
		LFbxAnimationLoader() {};
		void ParsingData(const FbxAnimStack* curAnimStack, LArray<LFbxAnimationStack>& outAnim, LFbxLoadContext context);
	private:
		FbxTimeSpan GetAnimationTimeSpan(const LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut, const FbxAnimStack* AnimStack);

		void FindAllAnimateNode(fbxsdk::FbxNode* rootNode, const FbxAnimStack* curAnimStack,LUnorderedMap<LString,fbxsdk::FbxNode*> &allNodeOut);

		bool CheckNodeHasTransformAnimation(fbxsdk::FbxNode* targetNode, FbxAnimLayer* animLayer);

		void GetNodeFromAnimLayer(fbxsdk::FbxNode* parentNode, FbxAnimLayer* animLayer, LUnorderedMap<LString, fbxsdk::FbxNode*>& allNodeOut);
	};

}