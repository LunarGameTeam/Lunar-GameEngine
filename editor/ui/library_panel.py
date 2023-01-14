import os

import luna
from core.editor_module import platform_module
from luna import imgui
from ui.panel import PanelBase


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


class LibraryPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.title = "资源库"
        self.asset_path = os.path.join(luna.get_module(luna.PlatformModule).engine_dir, "assets")
        self.asset_root = FolderInfo(self.asset_path)

    def on_imgui_folder(self, cur_item: 'FileInfo'):
        flag = 0
        if isinstance(cur_item, FolderInfo):
            pass
        elif isinstance(cur_item, FileInfo):
            flag = imgui.ImGuiTreeNodeFlags_Leaf

        clicked, expand = imgui.tree_node_callback(id(cur_item), flag)

        # if luna.imgui.begin_drag_drop_souce(0):
        #     luna.imgui.text(cur_item.name)
        #     luna.imgui.set_drag_drop_payload("LibraryItem", cur_item, 0)
        #     luna.imgui.end_drag_drop_souce()

        if isinstance(cur_item, FolderInfo):
            imgui.text("{} {}".format(imgui.ICON_FA_FOLDER, cur_item.name))
        else:
            imgui.text("{} {}".format(imgui.ICON_FA_FILE, cur_item.name))

        if expand:
            if isinstance(cur_item, FolderInfo):
                for child in cur_item.child_list:
                    self.on_imgui_folder(child)
            imgui.tree_pop()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        for f in self.asset_root.child_list:
            self.on_imgui_folder(f)
