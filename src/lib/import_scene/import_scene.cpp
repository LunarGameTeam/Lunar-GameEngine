#define RESOURCE_IMPORT_EXPORT
#include"import_scene.h"
namespace luna::resimport
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

	float LImportScene::GetUnitValue(LImportUnitType unit)
	{
		switch (unit)
		{
		case luna::resimport::ImportUnitMeter:
			return 1.0f;
			break;
		case luna::resimport::ImportUnitDeciMeter:
			return 0.1f;
			break;
		case luna::resimport::ImportUnitCenterMeter:
			return 0.01f;
			break;
		case luna::resimport::ImportUnitInch:
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
		case luna::resimport::ImportAxisZupRightHand:
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
		case luna::resimport::ImportAxisYupRightHand:
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
		for (LImportSceneNode &each_node : mNodes)
		{
			LMatrix4f nodeTransformMatrix = LMath::MatrixCompose(each_node.mTranslation, each_node.mRotation, each_node.mScal);
			LMatrix4f newTransformMatrix = convertInvMatrix * nodeTransformMatrix * convertMatrix;
			LMath::MatrixDecompose(newTransformMatrix, each_node.mTranslation, each_node.mRotation, each_node.mScal);
		}
		//根据转换矩阵，转换所有资源数据
		for (auto each_data : mDatas)
		{
			each_data->ConvertDataAxisAndUnit(convertInvMatrix, convertMatrix);
		}
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