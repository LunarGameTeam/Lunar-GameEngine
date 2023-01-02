from collections import deque
from queue import Queue
from typing import Type, List, Any

import core.editor
import luna
from luna import imgui


class PyInspectorPanel(luna.editor.PanelBase):
    editor_list: list[core.editor.EditorBase]
    editor: core.editor.EditorBase
    selected_entity: luna.Entity
    world_sys: luna.SceneModule

    def __init__(self) -> None:
        super(PyInspectorPanel, self).__init__()
        self.title = "Inspector"
        self.selected_entity = None
        self.editor = None
        self.width = 50

    def set_editor(self, editor):
        self.editor = editor

    def imgui_menu(self):
        pass

    def on_imgui(self) -> None:
        super(PyInspectorPanel, self).on_imgui()
        if self.editor:
            self.editor.on_imgui()
        return
