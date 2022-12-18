#include "core/asset/binary_asset.h"
#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include <iostream>

namespace luna
{

RegisterTypeEmbedd_Imp(LBinaryWithHeadAsset)
{
	cls->Binding<Self>();
	BindingModule::Get("luna")->AddType(cls);

	cls->Ctor<LBinaryWithHeadAsset>();

};

LBinaryWithHeadAsset::~LBinaryWithHeadAsset()
{
}

void LBinaryWithHeadAsset::OnAssetFileRead(LSharedPtr<Dictionary> meta, LSharedPtr<LFile> file)
{
	const byte* data_header = file->GetData().data();
	int32_t all_head_size = *(int32_t*)data_header;
	data_header += sizeof(int32_t);
	byte* json_header = new byte[all_head_size + 1];
	memcpy(json_header, data_header, all_head_size * sizeof(byte));
	json_header[all_head_size] = '\0';
	data_header += all_head_size;
	Json::Value val;
	Dictionary::FromBinary(json_header, (all_head_size + 1) * sizeof(byte), val);
	std::string ceshi = (char*)json_header;
	Dictionary dict(val);
	
	OnAssetBinaryRead(data_header);
	OnLoad();
}

void LBinaryWithHeadAsset::OnAssetFileWrite(LSharedPtr<Dictionary> meta, LVector<byte>& data)
{
	Json::Value root;
	Dictionary dict(root);	
	BinarySerializer serializer(dict);
	Serialize(serializer);
	Json::StyledWriter fastWriter;
	std::string output = fastWriter.write(dict.GetJsonValue());
	LString content(output);
	BaseValueToByte((int32_t)content.Length(), data);
	size_t data_size = data.size();
	size_t offset = data_size;
	data_size += content.Length();
	data.resize(data_size);
	memcpy(data.data() + offset, content.c_str(), content.Length() * sizeof(char));
	OnAssetBinaryWrite(data);
}

}