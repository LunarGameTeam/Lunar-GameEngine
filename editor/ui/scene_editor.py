import os
import luna
from luna import editor


class PySceneEditor(editor.SceneEditor):
    def __init__(self) -> None:
        super(PySceneEditor, self).__init__()

    def on_imgui(self) -> None:
        super(PySceneEditor, self).on_imgui()
