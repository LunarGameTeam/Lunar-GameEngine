#include "core/asset/asset.h"
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"

namespace luna
{

RegisterTypeEmbedd_Imp(LBasicAsset)
{
	cls->Ctor<Self>();
	cls->BindingProperty<&Self::mAssetPath>("path");
	cls->Binding<Self>();

	BindingModule::Get("luna")->AddType(cls);

};

LBasicAsset::LBasicAsset()
{

}

LBasicAsset::~LBasicAsset()
{
}

void LBasicAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
}

void LBasicAsset::OnAssetFileWrite(LSharedPtr<Dictionary> meta, LArray<byte>& data)
{
}

void LBasicAsset::OnAssetRelease()
{

}

LString LBasicAsset::GetAssetPath()
{
	return mAssetPath;
}

RegisterTypeEmbedd_Imp(LTextAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

	cls->Ctor<LTextAsset>();
};

LTextAsset::LTextAsset()
{

}

void LTextAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	LBasicAsset::OnAssetFileRead(meta, file);
	auto begin = (const char *)file->GetData().data();
	mContent = LString(begin, begin + file->GetData().size());
}

RegisterTypeEmbedd_Imp(LBinaryAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

	cls->Ctor<LBinaryAsset>();
};


LBinaryAsset::LBinaryAsset()
{

}

void LBinaryAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	mData = file;
}

}