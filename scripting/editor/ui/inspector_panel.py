import luna
from luna import imgui
from editor.ui.panel import PanelBase
from editor.core.inspector_base import InspectorBase


class InspectorPanel(PanelBase):
    editor_list: list[InspectorBase]
    editor: InspectorBase
    selected_entity: luna.Entity
    world_sys: luna.GameModule

    def __init__(self) -> None:
        super().__init__()
        self.title = "Inspector"
        self.selected_entity = None
        self.editor = None
        self.width = 50

    def set_editor(self, editor):
        self.editor = editor

    def imgui_menu(self):
        if luna.imgui.begin_menu_bar():
            if luna.imgui.begin_menu("创建", True):
                pass
            luna.imgui.end_menu_bar()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        self.imgui_menu()
        if self.editor and self.editor.target:
            self.editor.on_imgui()
        return
