import os
import luna
from luna import editor
from luna import imgui


class PyScenePanel(editor.ScenePanel):
    def __init__(self) -> None:
        super(PyScenePanel, self).__init__()
        self.scene = None
        self.has_menubar = True
        self.scene_texture = None

    def set_scene(self, scene):
        self.scene = scene
        from core.editor_core import render_module
        self.scene_texture = render_module.get_imgui_texture("SceneColor")

    def on_imgui(self) -> None:
        super(PyScenePanel, self).on_imgui()
        content = luna.imgui.get_content_region_avail()
        luna.imgui.image(self.scene_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))
        if luna.imgui.begin_menu_bar():
            if luna.imgui.begin_menu("创建", True):
                if imgui.menu_item("Cube"):
                    pass
                if imgui.menu_item("Sphere"):
                    pass
                luna.imgui.end_menu()
            luna.imgui.end_menu_bar()
