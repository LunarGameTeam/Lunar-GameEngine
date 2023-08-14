#include "AssetImport/AssetImportCommon.h"
#include "Graphics/Asset/MeshAsset.h"
#include "Graphics/Asset/SkeletalMeshAsset.h"
#include "Animation/Asset/SkeletonAsset.h"
#include "Animation/Asset/AnimationClipAsset.h"
namespace luna::asset
{
	RegisterTypeEmbedd_Imp(LSceneAssetNodeMessage)
	{
		cls->Ctor<LSceneAssetNodeMessage>();
		cls->Binding<Self>();
		cls->BindingMethod<&LSceneAssetNodeMessage::AddNodeToAsset>("add_node_to_asset");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetAssetNodeCount>("get_asset_node_count");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetNodeName>("get_node_name");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetNodeMeshAssetName>("get_node_mesh_asset_name");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetNodeMatAssetName>("get_node_mat_asset_name");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetNodeTranslation>("get_node_translation");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetNodeRotation>("get_node_rotation");
		cls->BindingMethod<&LSceneAssetNodeMessage::GetNodeScale>("get_node_scale");
		BindingModule::Luna()->AddType(cls);
	}

	void LAssetPack::SerializeAllAsset(const LPath& path)
	{
		LString rootPath = path.AsString();
		for (Asset* assetValue : mAssetDatas)
		{
			LString newPath = rootPath + "/" + assetValue->GetObjectName();
			if (assetValue->GetClass()->GetName() == LType::Get<graphics::MeshAsset>()->GetName())
			{
				newPath = newPath + ".lmesh";
			}
			sAssetModule->SaveAsset(assetValue, newPath);
		}
	}

	void LAssetPack::SerializeAllAsset(const LPath& path, const LString& assetName)
	{
		LString rootPath = path.AsString();
		for (Asset* assetValue : mAssetDatas)
		{
			LString newPath = rootPath + "/" + assetName;
			if (assetValue->GetClass()->GetName() == LType::Get<graphics::MeshAsset>()->GetName())
			{
				newPath = newPath + ".lmesh";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<graphics::SkeletalMeshAsset>()->GetName())
			{
				newPath = newPath + ".lskelmesh";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<animation::SkeletonAsset>()->GetName())
			{
				newPath = newPath + ".lskeleton";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<animation::AnimationClipAsset>()->GetName())
			{
				//track，默认让animation引用同路径下的同名动画,后面资源导入有界面这里就不需要了
				newPath = newPath + ".lskelanimclip";
			}
			sAssetModule->SaveAsset(assetValue, newPath);
		}
	}

	void LAssetPack::SerializeAllAssetToDemoScene(const LPath& path)
	{
		LString rootPath = path.AsString();
		for (Asset* assetValue : mAssetDatas)
		{
			LString newPath = rootPath + "/" + assetValue->GetObjectName();
			if (assetValue->GetClass()->GetName() == LType::Get<graphics::MeshAsset>()->GetName())
			{
				newPath = newPath + ".lmesh";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<graphics::SkeletalMeshAsset>()->GetName())
			{
				newPath = newPath + ".lskelmesh";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<animation::SkeletonAsset>()->GetName())
			{
				newPath = newPath + ".lskeleton";
			}
			else if (assetValue->GetClass()->GetName() == LType::Get<animation::AnimationClipAsset>()->GetName())
			{
				//track，默认让animation引用同路径下的同名动画,后面资源导入有界面这里就不需要了
				newPath = newPath + ".lskelanimclip";
			}
			sAssetModule->SaveAsset(assetValue, newPath);
		}
	}

	Asset* LAssetPack::GetAsset(size_t assetId)
	{
		assert(mAssetDatas.size() > assetId);
		return mAssetDatas[assetId];
	};

	void LAssetImportBase::ParsingImportScene(
		const LString& resSavePath,
		const asset::LImportScene& importSceneData,
		LAssetPack& outAssetPack,
		asset::LSceneAssetNodeMessage* nodeMessage
	)
	{
		ParsingImportSceneImpl(resSavePath,importSceneData, outAssetPack, nodeMessage);
	}
}