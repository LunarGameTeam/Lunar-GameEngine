#include "core/framework/application.h"

PyMODINIT_FUNC PyInit_luna()
{
	luna::LApplication::Instance();
	luna::BindingModule::Get("luna")->Init();
	return luna::BindingModule::Get("luna")->GetBinding();
}
