#include "Core/Asset/Asset.h"
#include "Core/Serialization/JsonSerializer.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Asset/AssetModule.h"

namespace luna
{

RegisterTypeEmbedd_Imp(Asset)
{
	cls->Ctor<Self>();
	cls->BindingProperty<&Self::mAssetPath>("path");
		
	cls->Binding<Self>();

	BindingModule::Get("luna")->AddType(cls);

};

Asset::Asset()
{

}

Asset::~Asset()
{
	sAssetModule->RemoveAsset(this);
}

void Asset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
}

void Asset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
{
}

void Asset::OnAssetRelease()
{

}

LString Asset::GetAssetPath()
{
	return mAssetPath;
}

RegisterTypeEmbedd_Imp(TextAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

	cls->Ctor<TextAsset>();
};

TextAsset::TextAsset()
{

}

void TextAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	Asset::OnAssetFileRead(meta, file);
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

void LBinaryAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	mData = file;
}

}