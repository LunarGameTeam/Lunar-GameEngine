import os
import luna
from core.editor_core import platform_module
from luna import editor


class FileInfo(object):
    def __init__(self, file_path):
        self.abs_path = file_path
        self.path = os.path.relpath(file_path, platform_module.engine_dir)
        self.name = os.path.basename(file_path)


class FolderInfo(FileInfo):
    def __init__(self, dir_path):
        super().__init__(dir_path)
        self.child_list = []
        self.init()

    def init(self):
        for f in os.listdir(self.abs_path):
            abs_path = os.path.join(self.abs_path, f)
            if os.path.isdir(abs_path):
                d = FolderInfo(abs_path)
                self.child_list.append(d)
            elif os.path.isfile(abs_path):
                file = FileInfo(abs_path)
                self.child_list.append(file)


class PyLibraryPanel(editor.PanelBase):
    def __init__(self) -> None:
        super(PyLibraryPanel, self).__init__()
        self.title = "资源库"
        self.asset_path = os.path.join(luna.get_module(luna.PlatformModule).engine_dir, "assets")
        self.asset_root = FolderInfo(self.asset_path)

    def on_imgui_folder(self, f: 'FileInfo'):

        if isinstance(f, FolderInfo):
            flag = 0
        elif isinstance(f, FileInfo):
            flag = luna.imgui.ImGuiTreeNodeFlags_Leaf

        def tree_node(hovered, held):

            if luna.imgui.begin_drag_drop_souce(0):
                luna.imgui.text(f.name)
                luna.imgui.set_drag_drop_payload("LibraryItem", f, 0)
                luna.imgui.end_drag_drop_souce()

            if isinstance(f, FolderInfo):
                luna.imgui.text("{} {}".format(luna.imgui.ICON_FA_FOLDER, f.name))
            else:
                luna.imgui.text("{} {}".format(luna.imgui.ICON_FA_FILE, f.name))

        if luna.imgui.tree_node_callback(id(f), flag, tree_node):
            if isinstance(f, FolderInfo):
                for f in f.child_list:
                    self.on_imgui_folder(f)
            else:
                pass
            luna.imgui.tree_pop()

    def on_imgui(self) -> None:
        super(PyLibraryPanel, self).on_imgui()
        for f in self.asset_root.child_list:
            self.on_imgui_folder(f)



