from typing import Type

import luna
from luna import *


class PyInspectorEditor(editor.InspectorEditor):
    selected_entity: Entity
    world_sys: SceneModule

    def __init__(self) -> None:
        super(PyInspectorEditor, self).__init__()
        self.selected_entity = None

    def set_selected(self, entity):
        self.selected_entity = entity

    def on_imgui(self) -> None:
        super(PyInspectorEditor, self).on_imgui()
        if self.selected_entity:
            count = self.selected_entity.get_component_count()
            for i in range(0, count):
                comp: luna.Component = self.selected_entity.get_component_at(i)
                if comp:
                    comp_type = comp.__class__
                    properties: list[LProperty] = comp_type.get_properties()
                    width = 50
                    for prop in properties:
                        width = max(luna.imgui.calc_text_size(prop.name))
                    for prop in properties:
                        luna.imgui.text(prop.name)
                        luna.imgui.same_line(50, 16)
                        if prop.type == luna.LVector3f:
                            try:
                                val = getattr(comp, prop.name)
                                changed, val = luna.imgui.drag_float3("##" + prop.name, val, 0.2, 1, -1)
                                if changed:
                                    setattr(comp, prop.name, val)
                            except Exception:
                                pass


