import os
import luna
from luna import editor


class PyLibraryPanel(editor.PanelBase):
    def __init__(self) -> None:
        super(PyLibraryPanel, self).__init__()
        self.title = "资源库"

    def on_imgui(self) -> None:
        super(PyLibraryPanel, self).on_imgui()
