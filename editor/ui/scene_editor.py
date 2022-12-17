import os
import luna
from luna import editor


class PySceneEditor(editor.SceneEditor):
    def __init__(self) -> None:
        super(PySceneEditor, self).__init__()
        self.scene = None
    def set_scene(self, scene):
        self.scene = scene
        pass

    def on_imgui(self) -> None:
        super(PySceneEditor, self).on_imgui()
