#include "core/reflection/reflection.h"
#include "core/binding/binding_traits.h"

namespace luna
{

RegisterType_Imp(bool, bool)
{
	Py_XINCREF(&PyBool_Type);
	cls->m_binding_type = &PyBool_Type;
}

RegisterType_Imp(const char*, const_char)
{
	Py_XINCREF(&PyUnicode_Type);
	cls->m_binding_type = &PyUnicode_Type;
}

RegisterType_Imp(int, int)
{
	Py_XINCREF(&PyLong_Type);
	cls->m_binding_type = &PyLong_Type;
}

RegisterType_Imp(float, float)
{
	Py_XINCREF(&PyFloat_Type);
	cls->m_binding_type = &PyFloat_Type;
}

RegisterType_Imp(double, double)
{
	Py_XINCREF(&PyFloat_Type);
	cls->m_binding_type = &PyFloat_Type;
}

RegisterType_Imp(LString, LString)
{
	Py_XINCREF(&PyUnicode_Type);
	cls->m_binding_type = &PyUnicode_Type;
}
}