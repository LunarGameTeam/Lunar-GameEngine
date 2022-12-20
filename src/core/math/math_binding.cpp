#include "core/math/math_binding.h"

#include "core/framework/module.h"
#include "core/core_library.h"
#include "core/framework/luna_core.h"

#include "core/object/base_object.h"
#include "core/foundation/log.h"

#include "core/foundation/misc.h"
#include "core/foundation/container.h"
#include "core/foundation/signal.h"
#include "core/foundation/string.h"

#include "core/memory/ptr.h"
#include "core/foundation/config.h"

#include "math.h"

namespace luna
{


using namespace binding;


template<typename T>
static PyObject* __add__(BindingStruct<T>* left, BindingStruct<T>* right)
{
	BindingStruct<T>* ret = struct_binding_proxy<T>::new_binding_object();
	ret->val = left->val + right->val;
	return ret;
}

template<typename T>
static float get_x(T* self)
{
	return self->x();
}

template<typename T>
static void set_x(T* self, float val)
{
	self->x() = val;
}

template<typename T>
static float get_y(T* self)
{
	return self->y();
}

template<typename T>
static void set_y(T* self, float val)
{
	self->y() = val;
}

template<typename T>
static float get_z(T* self)
{
	return self->z();
}

template<typename T>
static void set_z(T* self, float val)
{
	self->z() = val;
}
template<typename T>
static float get_w(T* self)
{
	return self->w();
}

template<typename T>
static void set_w(T* self, float val)
{
	self->w() = val;
}

RegisterType_Imp(LVector3f, LVector3f)
{
	cls->Binding<LVector3f>();
	cls->VirtualProperty("x")
		.Getter<get_x<LVector3f>>()
		.Setter<set_x<LVector3f>>()
		.Binding<LVector3f, float>();
	cls->VirtualProperty("y")
		.Getter<get_y<LVector3f>>()
		.Setter<set_y<LVector3f>>()
		.Binding<LVector3f, float>();
	cls->VirtualProperty("z")
		.Getter<get_z<LVector3f>>()
		.Setter<set_z<LVector3f>>()
		.Binding<LVector3f, float>();
	
	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float, z: float):\n\tpass");

	PyTypeObject* typeobject = cls->GetBindingType();	
	static PyNumberMethods methods;
	methods.nb_add = (binaryfunc)__add__<LVector3f>;
	typeobject->tp_as_number = &methods;

	BindingModule::Luna()->AddType(cls);
}

RegisterType_Imp(LVector2f, LVector2f)
{
	cls->Binding<LVector2f>();
	cls->VirtualProperty("x")
		.Getter<get_x<LVector2f>>()
		.Setter<set_x<LVector2f>>()
		.Binding<LVector2f, float>();
	cls->VirtualProperty("y")
		.Getter<get_y<LVector2f>>()
		.Setter<set_y<LVector2f>>()
		.Binding<LVector2f, float>();	

	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float):\n\tpass");

	PyTypeObject* typeobject = cls->GetBindingType();
	static PyNumberMethods methods;
	methods.nb_add = (binaryfunc)__add__<LVector2f>;
	typeobject->tp_as_number = &methods;

	BindingModule::Luna()->AddType(cls);
}


RegisterType_Imp(LQuaternion, LQuaternion)
{
	cls->Binding<LQuaternion>();
	cls->VirtualProperty("x")
		.Getter<get_x<LQuaternion>>()
		.Setter<set_x<LQuaternion>>()
		.Binding<LQuaternion, float>();
	cls->VirtualProperty("y")
		.Getter<get_y<LQuaternion>>()
		.Setter<set_y<LQuaternion>>()
		.Binding<LQuaternion, float>();
	cls->VirtualProperty("z")
		.Getter<get_z<LQuaternion>>()
		.Setter<set_z<LQuaternion>>()
		.Binding<LQuaternion, float>();
	cls->VirtualProperty("w")
		.Getter<get_w<LQuaternion>>()
		.Setter<set_w<LQuaternion>>()
		.Binding<LQuaternion, float>();
	BindingModule::Luna()->AddType(cls);

	BindingModule::Get("luna.math")->AddMethod<&LMath::FromEuler>("from_euler");
	BindingModule::Get("luna.math")->AddMethod<&LMath::ToEuler>("to_euler");

}

}
