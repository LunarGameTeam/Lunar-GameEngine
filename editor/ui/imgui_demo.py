import os
import luna
from core.editor_core import EditorCore
from luna import editor


class DemoPanel(editor.PanelBase):
    def __init__(self) -> None:
        super(DemoPanel, self).__init__()
        self.scene = None
        self.title = "Demo"

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self) -> None:
        luna.imgui.show_demo_window()
