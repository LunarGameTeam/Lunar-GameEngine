import os

import luna
from core.asset import FolderInfo, FileInfo
from core.editor_module import platform_module, EditorModule
from luna import imgui
from ui.panel import PanelBase


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
            if clicked:
                EditorModule.instance().open_asset(cur_item)

        if expand:
            if isinstance(cur_item, FolderInfo):
                for child in cur_item.child_list:
                    self.on_imgui_folder(child)
            imgui.tree_pop()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        for f in self.asset_root.child_list:
            self.on_imgui_folder(f)
