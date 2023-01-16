#include "Core/Asset/JsonAsset.h"
#include "Core/Serialization/JsonSerializer.h"
#include "Core/Reflection/Reflection.h"
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

void JsonAsset::OnAssetFileRead(LSharedPtr<JsonDict> meta, LSharedPtr<LFile> file)
{
	Json::Value val;
	LUNA_ASSERT(JsonDict::FromBinary(file->GetData().data(), file->GetData().size(), val));
	JsonDict dict(val);
	JsonSerializer serializer(dict);
	DeSerialize(serializer);
	OnLoad();
}

void JsonAsset::OnAssetFileWrite(LSharedPtr<JsonDict> meta, LArray<byte>& data)
{
	Json::Value root;
	JsonDict dict(root);	
	JsonSerializer serializer(dict);
	Serialize(serializer);
	Json::StyledWriter fastWriter;
	std::string output = fastWriter.write(dict.GetJsonValue());
	LString content(output);
	data.assign(content.std_str().begin(), content.std_str().end());
}

}