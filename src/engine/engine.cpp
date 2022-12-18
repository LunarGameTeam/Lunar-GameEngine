#include "core/framework/application.h"

static luna::LApplication* app = nullptr;

PyMODINIT_FUNC PyInit_luna()
{
	luna::LApplication::Instance();

	luna::BindingModule::Get("luna")->Init();
	return luna::BindingModule::Get("luna")->GetBinding();
}