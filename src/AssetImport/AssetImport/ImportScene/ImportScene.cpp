#include"AssetImport/ImportScene/ImportScene.h"

namespace luna::asset
{
	void LImportScene::SetAxisAndUnit(LImportAxisType axis, LImportUnitType unit)
	{
		mAxis = axis;
		mUnit = unit;
	}

	void LImportScene::AddNodeData(LImportSceneNode node)
	{
		mNodes.push_back(node);
	}

	void LImportScene::ResetNodeData(size_t nodeIndex, asset::LImportNodeDataType dataType, size_t dataIndex)
	{
		mNodes[nodeIndex].mNodeData[dataType] = dataIndex;
	}

	float LImportScene::GetUnitValue(LImportUnitType unit)
	{
		switch (unit)
		{
		case luna::asset::ImportUnitMeter:
			return 1.0f;
			break;
		case luna::asset::ImportUnitDeciMeter:
			return 0.1f;
			break;
		case luna::asset::ImportUnitCenterMeter:
			return 0.01f;
			break;
		case luna::asset::ImportUnitInch:
			return 0.0254;
			break;
		default:
			break;
		}
		return 1.0f;
	}

	void LImportScene::ComputeConvertMatrix(LImportAxisType axis, LImportUnitType unit,LMatrix4f& convertMatrix, LMatrix4f& convertInvMatrix)
	{
		assert(axis == LImportAxisType::ImportAxisYupLeftHand);
		LMatrix4f axisTransformMatrix, invAxisTransformMatrix;
		axisTransformMatrix.setIdentity();
		switch (mAxis)
		{
		case luna::asset::ImportAxisZupRightHand:
			axisTransformMatrix(0, 0) = 1;
			axisTransformMatrix(0, 1) = 0;
			axisTransformMatrix(0, 2) = 0;
			axisTransformMatrix(0, 3) = 0;

			axisTransformMatrix(1, 0) = 0;
			axisTransformMatrix(1, 1) = 0;
			axisTransformMatrix(1, 2) = 1;
			axisTransformMatrix(1, 3) = 0;

			axisTransformMatrix(2, 0) = 0;
			axisTransformMatrix(2, 1) = 1;
			axisTransformMatrix(2, 2) = 0;
			axisTransformMatrix(2, 3) = 0;

			axisTransformMatrix(3, 0) = 0;
			axisTransformMatrix(3, 1) = 0;
			axisTransformMatrix(3, 2) = 0;
			axisTransformMatrix(3, 3) = 1;
			break;
		case luna::asset::ImportAxisYupRightHand:
			axisTransformMatrix(0, 0) = -1;
			axisTransformMatrix(0, 1) = 0;
			axisTransformMatrix(0, 2) = 0;
			axisTransformMatrix(0, 3) = 0;

			axisTransformMatrix(1, 0) = 0;
			axisTransformMatrix(1, 1) = 1;
			axisTransformMatrix(1, 2) = 0;
			axisTransformMatrix(1, 3) = 0;

			axisTransformMatrix(2, 0) = 0;
			axisTransformMatrix(2, 1) = 0;
			axisTransformMatrix(2, 2) = 1;
			axisTransformMatrix(2, 3) = 0;

			axisTransformMatrix(3, 0) = 0;
			axisTransformMatrix(3, 1) = 0;
			axisTransformMatrix(3, 2) = 0;
			axisTransformMatrix(3, 3) = 1;
			break;
		default:
			assert(false);
			break;
		}
		invAxisTransformMatrix = axisTransformMatrix.inverse();

		LMatrix4f unitTransformMatrix, invUnitTransformMatrix;
		float file_unit = GetUnitValue(mUnit);
		float target_unit = GetUnitValue(unit);
		unitTransformMatrix.setIdentity();
		float deltaScal = target_unit / file_unit;
		unitTransformMatrix(0, 0) = deltaScal;
		unitTransformMatrix(1, 1) = deltaScal;
		unitTransformMatrix(2, 2) = deltaScal;
		invUnitTransformMatrix = unitTransformMatrix.inverse();
		convertMatrix = axisTransformMatrix * unitTransformMatrix;
		convertInvMatrix = invUnitTransformMatrix * invAxisTransformMatrix;
	}

	void LImportScene::ConvertDataAxisAndUnit(LImportAxisType axis, LImportUnitType unit)
	{
		//根据坐标系和单位计算出转换矩阵
		LMatrix4f convertMatrix,convertInvMatrix;
		ComputeConvertMatrix(axis, unit,convertMatrix, convertInvMatrix);
		//根据转换矩阵，转换所有节点的变换矩阵
		for (LImportSceneNode &eachNode : mNodes)
		{
			LMatrix4f nodeTransformMatrix = LMath::MatrixCompose(eachNode.mTranslation, eachNode.mRotation, eachNode.mScal);
			LMatrix4f newTransformMatrix = convertInvMatrix * nodeTransformMatrix * convertMatrix;
			LMath::MatrixDecompose(newTransformMatrix, eachNode.mTranslation, eachNode.mRotation, eachNode.mScal);
		}
		//根据转换矩阵，转换所有资源数据
		for (auto eachData : mDatas)
		{
			bool ifReflect = false;
			if (mAxis == ImportAxisZupRightHand || axis == ImportAxisYupRightHand)
			{
				ifReflect = true;
			}
			eachData->ConvertDataAxisAndUnit(ifReflect,convertInvMatrix, convertMatrix);
		}
		//根据转换矩阵，转换所有动画数据
		for (auto eachAniamtion : mAnimations)
		{
			bool ifReflect = false;
			if (mAxis == ImportAxisZupRightHand || axis == ImportAxisYupRightHand)
			{
				ifReflect = true;
			}
			eachAniamtion->ConvertAnimationAxisAndUnit(ifReflect, convertInvMatrix, convertMatrix);
		}
	}

	LArray<size_t> LImportScene::FilterDataByType(LImportNodeDataType type)
	{
		LArray<size_t> out;
		for (int32_t i = 0; i < mDatas.size(); ++i)
		{
			if (mDatas[i]->GetType() == type)
			{
				out.push_back(i);
			}
		}
		return out;
	}

	void LImportScene::PostProcessData()
	{
		for (auto each_data : mDatas)
		{
			switch (each_data->GetType())
			{
			case LImportNodeDataType::ImportDataMesh:
			{
				LImportNodeDataMesh* meshPt = dynamic_cast<LImportNodeDataMesh*>(each_data.get());
				meshPt->GenerateMikkTspaceTangent();
				meshPt->ComputeVertexCache();
				break;
			}
			default:
				break;
			}
		}
	}

}