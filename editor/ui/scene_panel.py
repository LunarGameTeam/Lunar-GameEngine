import os
import luna
from luna import editor


class PyScenePanel(editor.ScenePanel):
    def __init__(self) -> None:
        super(PyScenePanel, self).__init__()
        self.scene = None
    def set_scene(self, scene):
        self.scene = scene
        pass

    def on_imgui(self) -> None:
        super(PyScenePanel, self).on_imgui()
