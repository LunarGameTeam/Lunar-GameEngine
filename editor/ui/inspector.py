from typing import Type

import luna
from luna import imgui
from luna import *


class PyInspectorEditor(editor.InspectorEditor):
    selected_entity: Entity
    world_sys: SceneModule

    def __init__(self) -> None:
        super(PyInspectorEditor, self).__init__()
        self.selected_entity = None
        self.width = 50

    def set_selected(self, entity):
        self.selected_entity = entity

    def imgui_obj_property(self, comp, prop):
        if prop.name == "name":
            return
        luna.imgui.text(prop.name)
        val = getattr(comp, prop.name)
        if issubclass(prop.type, luna.LBasicAsset):
            changed, new_val = luna.imgui.input(val.path, 0)
            if changed:
                print(new_val)
        if issubclass(prop.type, luna.LObject):
            self.imgui_property(val)
        if prop.type == luna.LQuaternion:
            imgui.same_line(16 + self.width, 16)
            euler = luna.math.to_euler(val)
            changed, new_val = luna.imgui.drag_float3("##" + prop.name, euler, 0.2, 1, -1)
            if changed:
                q = luna.math.from_euler(new_val)
                setattr(comp, prop.name, q)
        elif prop.type == bool:
            imgui.same_line(16 + self.width, 16)
            changed, new_val = luna.imgui.checkbox("##" + prop.name, val)
            if changed:
                setattr(comp, prop.name, new_val)
        elif prop.type == luna.LVector3f:
            imgui.same_line(16 + self.width, 16)
            changed, new_val = luna.imgui.drag_float3("##" + prop.name, val, 0.2, 1, -1)
            if changed:
                setattr(comp, prop.name, new_val)

    def imgui_property(self, obj):
        comp_type = obj.__class__
        is_open = imgui.tree_node(obj.__repr__(), imgui.ImGuiTreeNodeFlags_DefaultOpen, comp_type.__name__)
        if is_open:
            properties: list[LProperty] = comp_type.get_properties()
            self.width = 50
            for prop in properties:
                self.width = max(luna.imgui.calc_text_size(prop.name).x, self.width)
            for prop in properties:
                self.imgui_obj_property(obj, prop)
            imgui.tree_pop()

    def imgui_menu(self):
        pass

    def on_imgui(self) -> None:
        super(PyInspectorEditor, self).on_imgui()
        if self.selected_entity:

            count = self.selected_entity.get_component_count()
            for i in range(0, count):
                comp: luna.Component = self.selected_entity.get_component_at(i)
                if not comp:
                    continue
                self.imgui_property(comp)
            self.imgui_menu()
