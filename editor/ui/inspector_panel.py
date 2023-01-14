import core.editor
import luna
from ui.panel import PanelBase


class InspectorPanel(PanelBase):
    editor_list: list[core.editor.EditorBase]
    editor: core.editor.EditorBase
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
        pass

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        if self.editor and self.editor.target:
            self.editor.on_imgui()
        return
