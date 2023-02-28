# encoding:utf-8
import time

import luna

from editor.core.asset import FileInfo
from editor.core.hot_patch import reload_module

asset_module: 'luna.AssetModule' = luna.get_module(luna.AssetModule)
render_module: 'luna.RenderModule' = luna.get_module(luna.RenderModule)
game_module: 'luna.GameModule' = luna.get_module(luna.GameModule)
platform_module: 'luna.PlatformModule' = luna.get_module(luna.PlatformModule)

if not __import__:
    from editor.ui.panel import WindowBase


def update_asset(path, asset_type):
    asset = asset_module.load_asset(path, asset_type)
    asset_module.save_asset(asset, path)


class EditorModule(luna.LModule):
    main_scene_window: 'WindowBase'
    _instance = None

    def __init__(self):
        super().__init__()

        self.project_dir = luna.get_config("DefaultProject")
        self.default_scene_path = luna.get_config("DefaultScene")
        self.main_scene_window = None
        self.material_window = None
        self.now = time.time()
        self.reload_module = set()

        if False:
            update_asset("/assets/test.scn", luna.Scene)
            update_asset("/assets/built-in/Skybox/Skybox.mat", luna.MaterialTemplateAsset)
            update_asset("/assets/built-in/Skybox/Skybox.cubemap", luna.TextureCube)
            update_asset("/assets/built-in/Pbr.mat", luna.MaterialTemplateAsset)

    def open_asset(self, f: 'FileInfo'):
        if not self.material_window:
            from editor.ui import MaterialWindow
            self.material_window = MaterialWindow()

    def on_load(self):
        pass

    def on_shutdown(self):
        luna.set_config("DefaultProject", self.project_dir)
        if self.main_scene_window.main_scene:
            luna.set_config("DefaultScene", self.main_scene_window.main_scene.path)

    def on_init(self):

        from editor.ui.scene_window import MainPanel
        from editor.ui.scene_window import generate_doc_for_module

        global asset_module, game_module, render_module, platform_module

        self.main_scene_window: 'MainPanel' = MainPanel()

        generate_doc_for_module(luna)

        if not self.project_dir:
            self.main_scene_window.show_message_box("提示", "请点击文件打开项目")
        elif not self.default_scene_path:
            self.main_scene_window.show_message_box("提示", "请点击文件打开默认场景")

        if self.default_scene_path and self.project_dir:
            scn = asset_module.load_asset(self.default_scene_path, luna.Scene)
            self.main_scene_window.set_main_scene(scn)

    def on_tick(self, delta_time):
        pass

    def on_imgui(self):
        if self.reload_module:
            for m in self.reload_module:
                reload_module(m)
            self.reload_module.clear()
            self.main_scene_window.show_status("重载Python代码")

        now = time.time()
        delta = now - self.now
        self.main_scene_window.do_imgui(delta)
        if self.material_window:
            self.material_window.do_imgui(delta)
        self.now = now

    @staticmethod
    def instance() -> 'EditorModule':
        if EditorModule._instance is None:
            EditorModule._instance = EditorModule()

        return EditorModule._instance
