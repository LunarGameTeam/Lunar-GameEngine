#pragma once

#include "core/misc/string.h"

namespace luna
{

struct Type
{
	LString Name;
	Type *Base;
public:
	bool IsSubClassOf(Type *parent)
	{
		Type *tmp = this;
		while (tmp != nullptr)
		{
			if (tmp == parent)
			{
				return true;
			}
			tmp = tmp->Base;
		}
		return false;
	};

private:
	Type()
	{

	}
};

template<typename T>
Type *NewType(const char *name, Type *base = nullptr)
{
	Type *type = new Type();
	type->Name = name;
	type->Base = base;
}

#define DECLARE_CLASS(ClassName,SuperClass) NewType<ClassName>(#ClassName, SuperClass);
}