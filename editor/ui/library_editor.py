import os
import luna
from luna import editor


class PyLibraryEditor(editor.LibraryEditor):
    def __init__(self) -> None:
        super(PyLibraryEditor, self).__init__()

    def on_imgui(self) -> None:
        super(PyLibraryEditor, self).on_imgui()
