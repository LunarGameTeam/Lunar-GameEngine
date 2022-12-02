#include "editor/ui/property_helper.h"

#include "editor/main_editor.h"

#include "core/object/component.h"
#include "core/object/entity.h"
#include "core/object/transform.h"
#include "core/event/EventModule.h"
#include "core/asset/asset_module.h"
#include "window/window.h"

#include "engine/scene_module.h"
#include "engine/camera.h"


namespace luna::editor
{
template <>
struct property_editor<LQuaternion>
{
	static void proerty_helper(LProperty* prop, LObject* target)
	{
		LType* prop_type = prop->GetType();
		LType* target_type = target->GetClass();
		LQuaternion& prop_val = prop->GetValue<LQuaternion>(target);
		LVector3f euler = LMath::ToEuler(prop_val);
		float float_val[3];
		float_val[0] = euler.x();
		float_val[1] = euler.y();
		float_val[2] = euler.z();
		if (ImGui::DragFloat3("##" + prop->GetName(), float_val, 0.2, 1, -1))
		{
			LVector3f new_val(float_val[0], float_val[1], float_val[2]);
			LQuaternion q = LMath::FromEuler(new_val);
			prop->SetValue(target, q);
		}
	}
};


template <>
struct property_editor<LString>
{
	static void proerty_helper(LProperty* prop, LObject* target)
	{
		LType* prop_type = prop->GetType();
		LType* target_type = target->GetClass();
		LString& prop_val = prop->GetValue<LString>(target);
		InputLString("##" + prop->GetName(), &prop_val, ImGuiInputTextFlags_CallbackAlways);
	}
};


template <>
struct property_editor<LVector3f>
{
	static void proerty_helper(LProperty* prop, LObject* target)
	{
		LType* prop_type = prop->GetType();
		LType* target_type = target->GetClass();
		LVector3f& prop_val = prop->GetValue<LVector3f>(target);
		float float_val[3];
		float_val[0] = prop_val.x();
		float_val[1] = prop_val.y();
		float_val[2] = prop_val.z();
		ImGui::DragFloat3("##" + prop->GetName(), float_val, 0.2f, 1.0f, -1.0f);
		LVector3f new_val(float_val[0], float_val[1], float_val[2]);
		prop->SetValue(target, new_val);
	}
};


template <>
struct property_editor<float>
{
	static void proerty_helper(LProperty* prop, LObject* target)
	{
		LType* prop_type = prop->GetType();
		LType* target_type = target->GetClass();
		float& prop_val = prop->GetValue<float>(target);
		float val = prop_val;
		ImGui::DragFloat("##float", &val);
		if (val != prop_val)
			prop->SetValue(target, val);
	}
};


template <>
struct property_editor<bool>
{
	static void proerty_helper(LProperty* prop, LObject* target)
	{
		LType* prop_type = prop->GetType();
		LType* target_type = target->GetClass();
		bool& prop_val = prop->GetValue<bool>(target);
		bool val = prop_val;
		ImGui::Checkbox("##bool", &val);
		if (val != prop_val)
			prop->SetValue(target, val);
	}
};


void property_helper_func(LProperty* prop, LObject* target)
{
	static std::map<LType*, helper_func_signature> property_helper_map =
	{
		{LType::Get<LQuaternion>(), &property_editor<LQuaternion>::proerty_helper},
		{LType::Get<LString>(), &property_editor<LString>::proerty_helper},
		{LType::Get<LVector3f>(), &property_editor<LVector3f>::proerty_helper},
		{LType::Get<bool>(), &property_editor<bool>::proerty_helper},
	};
	LType* prop_type = prop->GetType();
	LType* target_type = target->GetClass();

	if (prop->GetName() == "name")
		return;

	helper_func_signature helper_func = property_helper_map[prop_type];
	if (helper_func)
	{
		helper_func(prop, target);
	}
	else if (prop_type->IsSubPtrArray())
	{
		TSubPtrArray<LObject>& prop_val = prop->GetValue<TSubPtrArray<LObject>>(target);
		LType* temp_type = prop_type->GetTemplateArg();
		int index = 0;
		for (TSubPtr<LObject>& ptr : prop_val)
		{
			LObject* obj_elem = ptr.Get();
			if (ImGui::TreeNodeEx(obj_elem, 0, std::to_string(index).c_str()))
			{
				std::vector<LProperty*> obj_properties;
				obj_elem->GetClass()->GetAllProperties(obj_properties);
				if (target)
				{
					for (LProperty* obj_prop : obj_properties)
					{
						property_helper_func(obj_prop, obj_elem);
					}
				}
				ImGui::TreePop();
			}
			index++;
		}
	}
	else if (prop->IsSubPointer())
	{
		LSubPtr& prop_val = prop->GetValue<LSubPtr>(target);
		LType* temp_type = prop_type->GetTemplateArg();
		LObject* prop_obj = prop_val.Get();
		if (temp_type->IsDerivedFrom(LType::Get<LBasicAsset>()))
		{
			LBasicAsset* asset = dynamic_cast<LBasicAsset*>(prop_obj);
			LString path;
			if (asset)
				path = asset->GetAssetPath();
			if (InputLString("path/" + prop->GetName(), &path,
			                 ImGuiInputTextFlags_CallbackAlways | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				asset = sAssetModule->LoadAsset(path, temp_type);
				LSubPtr value(target);
				value.SetPtr(asset);
				if (prop->GetSetter().Valid())
					prop->SetValue(target, asset);
				else
					prop->SetValue(target, value);
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("LIBRARY"))
				{
					LFileInfo* info = *((LFileInfo**)payload->Data);
					if (info)
					{
						path = info->mEnginePath;
						asset = sAssetModule->LoadAsset(path, temp_type);
						LSubPtr value(target);
						value.SetPtr(asset);
						if (prop->GetSetter().Valid())
							prop->SetValue(target,  asset);
						else
							prop->SetValue(target, value);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
		else
		{
			std::vector<LProperty*> obj_properties;
			ImGui::BeginGroup();
			temp_type->GetAllProperties(obj_properties);
			if (prop_obj)
			{
				for (LProperty* obj_prop : obj_properties)
				{
					property_helper_func(obj_prop, prop_obj);
				}
			}
			ImGui::EndGroup();
		}
	}
}
}
