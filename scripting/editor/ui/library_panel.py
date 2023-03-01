import os

import luna
from editor.core.asset import FolderInfo, FileInfo
from editor.core.editor_module import EditorModule, asset_module
from editor.core.inspector_base import create_inspector
from luna import imgui
from editor.ui.panel import PanelBase

import luna
from editor.core.editor_module import asset_module
file_icon = asset_module.load_asset("assets/built-in/Editor/File.png", luna.Texture2D)


class LibraryPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.title = "资源库"
        self.engine_asset_path = os.path.join(luna.get_module(luna.PlatformModule).engine_dir, "assets")
        self.project_asset_path = os.path.join(luna.get_module(luna.PlatformModule).project_dir, "assets")
        self.engine_root = FolderInfo(self.engine_asset_path)
        self.proj_root = FolderInfo(self.project_asset_path)

    def on_imgui_folder(self, cur_item: 'FileInfo'):
        flag = 0
        if isinstance(cur_item, FolderInfo):
            pass
        elif isinstance(cur_item, FileInfo):
            flag = imgui.ImGuiTreeNodeFlags_Leaf

        clicked, expand, double_click = imgui.tree_node_callback(id(cur_item), luna.LVector2f(-1, 32), flag)

        if luna.imgui.begin_drag_drop_souce(0):
            luna.imgui.text(cur_item.name)
            luna.imgui.set_drag_drop_payload("LibraryItem", cur_item, 0)
            luna.imgui.end_drag_drop_souce()

        imgui.text("{} {}".format(cur_item.asset_type.icon, cur_item.name))

        if not isinstance(cur_item, FolderInfo):
            if double_click:
                cur_item.asset_type.on_double_click(cur_item)


        if expand:
            if isinstance(cur_item, FolderInfo):
                for child in cur_item.child_list:
                    self.on_imgui_folder(child)
            imgui.tree_pop()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        flag = luna.imgui.ImGuiTreeNodeFlags_DefaultOpen
        clicked, expand, double_click = imgui.tree_node_callback(id(self.engine_root), luna.LVector2f(-1, 32), flag)
        imgui.text("Engine")
        if expand:
            for f in self.engine_root.child_list:
                self.on_imgui_folder(f)
            imgui.tree_pop()

        clicked, expand, double_click = imgui.tree_node_callback(id(self.proj_root), luna.LVector2f(-1, 32), flag)
        imgui.text("Project")
        if expand:
            for f in self.proj_root.child_list:
                self.on_imgui_folder(f)
            imgui.tree_pop()

