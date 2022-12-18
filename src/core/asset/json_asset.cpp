#include "core/asset/json_asset.h"
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include <iostream>

namespace luna
{

RegisterTypeEmbedd_Imp(LJsonAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

	cls->Ctor<LJsonAsset>();

};

LJsonAsset::~LJsonAsset()
{
}

void LJsonAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	Json::Value val;
	assert(Dictionary::FromBinary(file->GetData().data(), file->GetData().size(), val));
	Dictionary dict(val);
	JsonSerializer serializer(dict);
	DeSerialize(serializer);
	OnLoad();
}

void LJsonAsset::OnAssetFileWrite(LSharedPtr<Dictionary> meta, LVector<byte>& data)
{
	Json::Value root;
	Dictionary dict(root);	
	JsonSerializer serializer(dict);
	Serialize(serializer);
	Json::StyledWriter fastWriter;
	std::string output = fastWriter.write(dict.GetJsonValue());
	LString content(output);
	data.assign(content.str().begin(), content.str().end());
}

}