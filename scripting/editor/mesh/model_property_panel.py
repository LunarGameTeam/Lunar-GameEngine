import luna
from luna import imgui
from editor.core.inspector_base import InspectorBase
from editor.custom_widget.custom_child_panel import CustomChildPanelBase

class PropertyInspector(InspectorBase):
    def __init__(self, target):
        super().__init__(target)

    def on_imgui(self):
        if(self.target == None):
            return
        target_type = self.target.__class__
        properties = target_type.get_properties()
        if self.on_category():
            for prop in properties:
                self.imgui_obj_property(self.target, prop.name, prop.type)
            imgui.tree_pop()
        imgui.separator()

class ModelPropertyPanel(CustomChildPanelBase):
    editor_list: list[InspectorBase]
    editor: InspectorBase
    selected_entity: luna.Entity
    world_sys: luna.GameModule

    def __init__(self) -> None:
        super().__init__(self)
        self.title = "property"
        self.selected_entity = None
        self.editor = PropertyInspector(None)
        self.width = 50

    def set_target(self, target_in):
        self.editor.target = target_in

    def imgui_menu(self):
        pass

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        self.imgui_menu()
        if self.editor and self.editor.target:
            self.editor.on_imgui()
        return