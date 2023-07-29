import inspect
import os
import tkinter
import types

import luna
from editor.core.editor_module import EditorModule, game_module, asset_module, platform_module
from editor.ui.panel import WindowBase
from luna import imgui


class MaterialWindow(WindowBase):
    main_scene: 'luna.Scene' = None
    window_name = "Scene Window"

    def __init__(self) -> None:
        super().__init__()

        from editor.ui.inspector_panel import InspectorPanel
        from editor.ui.scene_panel import ScenePanel
        from editor.ui.library_panel import LibraryPanel

        self.library_panel = self.add_panel(LibraryPanel)
        self.inspector = self.add_panel(InspectorPanel)

    def on_title(self):
        proj_dir = EditorModule.instance().project_dir
        scene_path = EditorModule.instance().default_scene_path
        app_title = imgui.ICON_FA_MOON + "  Luna Editor"
        self.title = "{}\t\t{}\t\t{}".format(app_title, proj_dir, scene_path)

    def on_toolbar_menu(self):
        super().on_toolbar_menu()
        if imgui.begin_menu("文件", True):
            imgui.end_menu()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
