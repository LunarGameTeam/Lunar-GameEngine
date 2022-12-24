#include "core/reflection/reflection.h"
#include "core/binding/binding_traits.h"

namespace luna
{

RegisterType_Imp(bool, bool)
{
	Py_XINCREF(&PyBool_Type);
	cls->mPyType = &PyBool_Type;
}

RegisterType_Imp(const char*, const_char)
{
	Py_XINCREF(&PyUnicode_Type);
	cls->mPyType = &PyUnicode_Type;
}

RegisterType_Imp(int, int)
{
	Py_XINCREF(&PyLong_Type);
	cls->mPyType = &PyLong_Type;
}

RegisterType_Imp(float, float)
{
	Py_XINCREF(&PyFloat_Type);
	cls->mPyType = &PyFloat_Type;
}

RegisterType_Imp(double, double)
{
	Py_XINCREF(&PyFloat_Type);
	cls->mPyType = &PyFloat_Type;
}

RegisterType_Imp(LString, LString)
{
	Py_XINCREF(&PyUnicode_Type);
	cls->mPyType = &PyUnicode_Type;
}
}