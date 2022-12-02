#include "core/framework/application.h"

static luna::LApplication* app = nullptr;

PyMODINIT_FUNC PyInit_luna()
{
	luna::LApplication::Instance();

	luna::LBindingModule::Get("luna")->Init();
	return luna::LBindingModule::Get("luna")->GetBinding();
}