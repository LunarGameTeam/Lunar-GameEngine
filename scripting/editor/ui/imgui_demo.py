import luna

from editor.ui.panel import PanelBase


class DemoPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.scene = None
        self.title = "Demo"

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self, delta_time) -> None:
        luna.imgui.show_demo_window()
