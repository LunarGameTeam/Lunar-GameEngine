import os
import luna
from core.editor_module import EditorModule
from luna import editor

from ui.panel import PanelBase

class DemoPanel(PanelBase):
    def __init__(self) -> None:
        super(DemoPanel, self).__init__()
        self.scene = None
        self.title = "Demo"

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self) -> None:
        luna.imgui.show_demo_window()
