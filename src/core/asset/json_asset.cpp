#include "core/asset/json_asset.h"
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include <iostream>

namespace luna
{

RegisterTypeEmbedd_Imp(JsonAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

	cls->Ctor<JsonAsset>();

};

JsonAsset::~JsonAsset()
{
}

void JsonAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	Json::Value val;
	LUNA_ASSERT(Dictionary::FromBinary(file->GetData().data(), file->GetData().size(), val));
	Dictionary dict(val);
	JsonSerializer serializer(dict);
	DeSerialize(serializer);
	OnLoad();
}

void JsonAsset::OnAssetFileWrite(LSharedPtr<Dictionary> meta, LArray<byte>& data)
{
	Json::Value root;
	Dictionary dict(root);	
	JsonSerializer serializer(dict);
	Serialize(serializer);
	Json::StyledWriter fastWriter;
	std::string output = fastWriter.write(dict.GetJsonValue());
	LString content(output);
	data.assign(content.std_str().begin(), content.std_str().end());
}

}