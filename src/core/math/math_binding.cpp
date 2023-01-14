#include "core/math/math_binding.h"
#include "core/core_library.h"
#include "core/object/base_object.h"
#include "core/foundation/container.h"
#include "core/foundation/string.h"


#include "math.h"

namespace luna
{


using namespace binding;


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

template<typename T>
float get_size(PyObject* obj)
{
	BindingStruct<T>* t = (BindingStruct<T>*)(obj);	
	return t->val.norm();
}

template<typename T>
void normarlize(PyObject* obj)
{
	BindingStruct<T>* t = (BindingStruct<T>*)(obj);
	return t->val.normalize();
}


template<typename T>
T zero()
{
	return T::Zero();
}

template<typename T>
T identity()
{
	return T::Identity();
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

	cls->BindingMethod<&get_size<LVector2f>>("size");
	cls->BindingMethod<&normarlize<LVector2f>>("normalize");
	cls->BindingMethod<&zero<LVector2f>, MethodType::StaticFunction>("zero");
	
	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float):\n\t\tsuper(LVector2f, self).__init__()");
	BindingModule::Luna()->AddType(cls);
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

	cls->BindingMethod<&get_size<LVector3f>>("size");
	cls->BindingMethod<&normarlize<LVector3f>>("normalize");
	cls->BindingMethod<&zero<LVector3f>, MethodType::StaticFunction>("zero");
	
	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float, z: float):\n\t\tsuper(LVector3f, self).__init__()");
	BindingModule::Luna()->AddType(cls);
}

RegisterType_Imp(LVector4f, LVector4f)
{
	cls->Binding<LVector4f>();
	cls->VirtualProperty("x")
		.Getter<get_x<LVector4f>>()
		.Setter<set_x<LVector4f>>()
		.Binding<LVector4f, float>();
	cls->VirtualProperty("y")
		.Getter<get_y<LVector4f>>()
		.Setter<set_y<LVector4f>>()
		.Binding<LVector4f, float>();
	cls->VirtualProperty("z")
		.Getter<get_z<LVector4f>>()
		.Setter<set_z<LVector4f>>()
		.Binding<LVector4f, float>();
	cls->VirtualProperty("w")
		.Getter<get_w<LVector4f>>()
		.Setter<set_w<LVector4f>>()
		.Binding<LVector4f, float>();

	cls->BindingMethod<&get_size<LVector4f>>("size");
	cls->BindingMethod<&normarlize<LVector4f>>("normalize");
	cls->BindingMethod<&zero<LVector4f>, MethodType::StaticFunction>("zero");

	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float, z: float, w: float):\n\t\tsuper(LVector4f, self).__init__()");
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

	cls->GetExtraDocs().push_back("def __init__(self, x: float, y: float, z: float, w: float):\n\t\tsuper(LQuaternion, self).__init__()");
	PyTypeObject* typeobject = cls->GetBindingType();

	BindingModule::Get("luna.math")->AddMethod<&LMath::FromEuler>("from_euler");
	BindingModule::Get("luna.math")->AddMethod<&LMath::ToEuler>("to_euler");
	BindingModule::Get("luna.math")->AddMethod<&LMath::AngleAxisf>("angle_axis");

	cls->BindingMethod<&normarlize<LQuaternion>>("normalize");

	BindingModule::Luna()->AddType(cls);

}

LVector3f translation(PyObject* obj)
{
	BindingStruct<LMatrix4f>* t = (BindingStruct<LMatrix4f>*)(obj);	
	return LMath::GetMatrixTranslaton(t->val);
}

LQuaternion rotation(PyObject* obj)
{
	BindingStruct<LMatrix4f>* t = (BindingStruct<LMatrix4f>*)(obj);
	return LMath::GetMatrixRotation(t->val);
}

RegisterType_Imp(LMatrix4f, LMatrix4f)
{
	cls->Binding<LMatrix4f>();
	cls->BindingMethod<&translation>("translation");
	cls->BindingMethod<&rotation>("rotation");
	cls->BindingMethod<&identity<LMatrix4f>, MethodType::StaticFunction>("identity");
	BindingModule::Luna()->AddType(cls);
}

}
