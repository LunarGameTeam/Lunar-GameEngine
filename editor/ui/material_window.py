import tkinter.filedialog

import luna
from core.editor_module import platform_module, EditorModule
from luna import imgui
from ui.panel import WindowBase


class MaterialWindow(WindowBase):
    main_scene: 'luna.Scene' = None
    window_name = "Material Window"

    def __init__(self) -> None:
        super().__init__()

        self.main_scene = None

        from ui.inspector_panel import InspectorPanel
        from ui.library_panel import LibraryPanel

        self.library_panel = self.add_panel(LibraryPanel)

    def on_title(self):
        proj_dir = EditorModule.instance().project_dir
        app_title = imgui.ICON_FA_MOON + "  Luna Material Editor"
        self.title = app_title

    def set_main_scene(self, scn):
        if not scn:
            return

    def on_file_menu(self):
        if imgui.begin_menu("文件", True):
            if imgui.menu_item("保存"):
                name = tkinter.filedialog.askdirectory(initialdir=platform_module.engine_dir)
                if name:
                    platform_module.set_project_dir(name)
                pass
            imgui.end_menu()

    def on_help_menu(self):
        if imgui.begin_menu("帮助", True):
            if imgui.menu_item("关于"):
                self.show_message_box("Luna Editor 0.1", "made by Isak Wong, Pancy Star")
            imgui.end_menu()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        if imgui.begin_menu_bar():
            self.on_file_menu()
            self.on_help_menu()
            imgui.end_menu_bar()
