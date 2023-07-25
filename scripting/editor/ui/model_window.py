import inspect
import os
import tkinter.filedialog
import types

import luna
from editor.core.editor_module import platform_module, EditorModule, asset_module, game_module
from luna import imgui
from editor.ui.custom_window_panel import CustomWindowPanel


class ModelEditWindow(CustomWindowPanel):
    def __init__(self,initwidth,initheight) -> None:
        super().__init__("ModelEditWindow",initwidth,initheight)
        #添加view窗口
        from editor.ui.custom_scene_view_pannel import CustomSceneViewPanel
        self.add_panel(CustomSceneViewPanel)
        from editor.ui.model_submesh_panel import ModelSubmeshPanel
        self.add_panel(ModelSubmeshPanel)


    def on_title(self):
        proj_dir = EditorModule.instance().project_dir
        scene_path = EditorModule.instance().default_scene_path
        app_title = imgui.ICON_FA_MOON + "  Luna Editor"
        self.title = "{}\t\t{}\t\t{}".format(app_title, proj_dir, scene_path)

    def on_file_menu(self):
        if imgui.begin_menu("文件", True):
            if imgui.menu_item("保存模型"):
                asset_module.save_asset(self.main_scene, self.main_scene.path)
            if imgui.menu_item("退出"):
                EditorModule.instance().open_asset(None)

            imgui.end_menu()

    def on_help_menu(self):
        if imgui.begin_menu("帮助", True):
            if imgui.menu_item("关于"):
                self.show_message_box("Luna Editor 0.1", "made by Isak Wong, Pancy Star")
            imgui.end_menu()

    def on_tool_menu(self):
        if imgui.begin_menu("工具", True):
            if imgui.menu_item("关于"):
                self.show_message_box("Luna Editor 0.1", "made by Isak Wong, Pancy Star")
            imgui.end_menu()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        if imgui.begin_menu_bar():
            self.on_file_menu()
            self.on_tool_menu()
            self.on_help_menu()
            imgui.end_menu_bar()