#include "path.h"
#include "string.h"

using namespace luna;

void LPathUtil::DivideFilePath(const LString &full_file_name_in, LString &file_path_out, LString &file_name_out, LString &file_tail_out)
{
	//获取文件后缀名
	LString now_file_path = full_file_name_in;
	size_t length_name = now_file_path.Length();
	while (now_file_path[length_name - 1] != '.')
	{
		length_name -= 1;
	}
	file_tail_out = now_file_path.Substr(length_name, now_file_path.Length() - length_name);
	length_name -= 1;
	//处理存储文件的文件名
	LString file_root_name = now_file_path.Substr(0, length_name);
	int32_t st_pos = 0;
	for (int32_t i = 0; i < file_root_name.Length(); ++i)
	{
		if (file_root_name[i] == '\\' || file_root_name[i] == '/')
		{
			st_pos = i + 1;
		}
	}
	if (st_pos < file_root_name.Length())
	{
		file_name_out = file_root_name.Substr(st_pos, file_root_name.Length() - st_pos);
	}
	else
	{
		file_name_out = "";
	}
	//获取文件的路径名
	file_path_out = file_root_name.Substr(0, st_pos);
}