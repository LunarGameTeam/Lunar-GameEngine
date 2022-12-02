#include "editor/ui/base_panel.h"
#include "core/object/entity.h"
#include "core/event/EventModule.h"

#include "window/window.h"

#include "engine/scene_module.h"


namespace luna::editor
{
using helper_func_signature = void(*)(LProperty*, LObject*);


template <typename T>
struct property_editor
{
	static void proerty_helper(LProperty* prop, LObject* target);
};


void property_helper_func(LProperty* prop, LObject* target);
}
