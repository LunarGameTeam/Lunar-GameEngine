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

    def imgui_component_property(self, comp, prop):
        if prop.name == "name":
            return
        luna.imgui.text(prop.name)
        val = getattr(comp, prop.name)
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

    def imgui_property(self):
        count = self.selected_entity.get_component_count()
        for i in range(0, count):
            comp: luna.Component = self.selected_entity.get_component_at(i)
            if not comp:
                continue

            comp_type = comp.__class__
            is_open = imgui.tree_node(comp.__repr__(), imgui.ImGuiTreeNodeFlags_DefaultOpen, comp_type.__name__)
            if is_open:
                properties: list[LProperty] = comp_type.get_properties()
                self.width = 50
                for prop in properties:
                    self.width = max(luna.imgui.calc_text_size(prop.name).x, self.width)
                for prop in properties:
                    self.imgui_component_property(comp, prop)
                imgui.tree_pop()

    def imgui_menu(self):
        pass

    def on_imgui(self) -> None:
        super(PyInspectorEditor, self).on_imgui()
        if self.selected_entity:
            self.imgui_property()
            self.imgui_menu()
