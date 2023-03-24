#include "Animation/Asset/AnimationClipAsset.h"
namespace luna::animation
{

RegisterTypeEmbedd_Imp(AnimationClipAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);
	cls->Ctor<AnimationClipAsset>();
};
void AnimationClipAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	const byte* data_header = file->GetData().data();
	size_t offset = 0;
	OnLoad();
};

void AnimationClipAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
{
	size_t globel_size = 0;
}
}
