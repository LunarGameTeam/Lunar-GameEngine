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

namespace luna
{


using namespace binding;

static PyObject* __add__(BindingStruct<LVector3f>* left, BindingStruct<LVector3f>* right)
{
	BindingStruct<LVector3f>* ret = struct_binding_proxy<LVector3f>::new_binding_object();
	ret->val = left->val + right->val;
	return ret;
}

static float get_x(LVector3f* self)
{
	return self->x();
}

static void set_x(LVector3f* self, float val)
{
	self->x() = val;
}

static float get_y(LVector3f* self)
{
	return self->y();
}

static void set_y(LVector3f* self, float val)
{
	self->y() = val;
}

static float get_z(LVector3f* self)
{
	return self->z();
}

static void set_z(LVector3f* self, float val)
{
	self->z() = val;
}

RegisterType_Imp(LVector3f, LVector3f)
{
	cls->Binding<LVector3f>();
	cls->VirtualProperty("x")
		.Getter<get_x>()
		.Setter<set_x>()
		.Binding<LVector3f, float>();
	cls->VirtualProperty("y")
		.Getter<get_y>()
		.Setter<set_y>()
		.Binding<LVector3f, float>();
	cls->VirtualProperty("z")
		.Getter<get_z>()
		.Setter<set_z>()
		.Binding<LVector3f, float>();
	
	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float, z: float):\n\tpass");

	PyTypeObject* typeobject = cls->GetBindingType();	
	static PyNumberMethods methods;
	methods.nb_add = (binaryfunc)__add__;
	typeobject->tp_as_number = &methods;

	LBindingModule::Luna()->AddType(cls);
}

}
