import core.editor
import luna
from ui.panel import PanelBase


class InspectorPanel(PanelBase):
    editor_list: list[core.editor.EditorBase]
    editor: core.editor.EditorBase
    selected_entity: luna.Entity
    world_sys: luna.GameModule

    def __init__(self) -> None:
        super(InspectorPanel, self).__init__()
        self.title = "Inspector"
        self.selected_entity = None
        self.editor = None
        self.width = 50

    def set_editor(self, editor):
        self.editor = editor

    def imgui_menu(self):
        pass

    def on_imgui(self) -> None:
        super(InspectorPanel, self).on_imgui()
        if self.editor and self.editor.target:
            self.editor.on_imgui()
        return
