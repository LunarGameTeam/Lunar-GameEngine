#include "Core/Framework/LunaCore.h"

PyMODINIT_FUNC PyInit_luna()
{
	luna::LunaCore::Ins();
	luna::BindingModule::Get("luna")->Init();
	return luna::BindingModule::Get("luna")->GetBinding();
}
