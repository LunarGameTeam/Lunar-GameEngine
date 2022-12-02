import os
import luna
from luna import editor


class PyHierarchyEditor(editor.HierarchyEditor):
    def __init__(self) -> None:
        super(PyHierarchyEditor, self).__init__()

    def on_imgui(self) -> None:
        super(PyHierarchyEditor, self).on_imgui()
