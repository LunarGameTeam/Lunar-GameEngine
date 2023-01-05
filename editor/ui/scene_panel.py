import os
import luna
from luna import editor


class PyScenePanel(editor.ScenePanel):
    def __init__(self) -> None:
        super(PyScenePanel, self).__init__()
        self.scene = None

    def set_scene(self, scene):
        from core.editor_core import render_module
        self.imgui_texture = render_module.get_imgui_texture("SceneColor")

    def on_imgui(self) -> None:
        super(PyScenePanel, self).on_imgui()
        content = luna.imgui.get_content_region_avail()
        luna.imgui.image(self.imgui_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))
