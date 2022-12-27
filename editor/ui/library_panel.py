import os
import luna
from luna import editor


class Directory(object):
    def __init__(self, dir_path):
        self.abs_path = dir_path
        self.name = os.path.basename(dir_path)
        self.child_list = []
        self.init()

    def init(self):
        for f in os.listdir(self.abs_path):
            abs_path = os.path.join(self.abs_path, f)
            if os.path.isdir(abs_path):
                d = Directory(abs_path)
                self.child_list.append(d)
            elif os.path.isfile(abs_path):
                file = FileInfo(abs_path)
                self.child_list.append(file)


class FileInfo(object):
    def __init__(self, file_path):
        self.abs_path = file_path
        self.name = os.path.basename(file_path)


class PyLibraryPanel(editor.PanelBase):
    def __init__(self) -> None:
        super(PyLibraryPanel, self).__init__()
        self.title = "资源库"
        self.asset_path = os.path.join(luna.get_module(luna.PlatformModule).engine_dir, "assets")
        self.asset_root = Directory(self.asset_path)

    def on_imgui_folder(self, folder: 'Directory'):
        def item_clicked(hovered, held):
            luna.imgui.text(folder.name)

        if self.custom_treenode(folder.name, 0, item_clicked):
            for f in folder.child_list:
                self.on_imgui_folder(f)
            luna.imgui.tree_pop()

    def on_imgui(self) -> None:
        super(PyLibraryPanel, self).on_imgui()
        for f in self.asset_root.child_list:
            self.on_imgui_folder(f)



