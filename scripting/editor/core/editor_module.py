# encoding:utf-8
import time

import luna
from editor.core.hot_patch import reload_module
from luna import imgui

asset_module: 'luna.AssetModule' = luna.get_module(luna.AssetModule)
render_module: 'luna.RenderModule' = luna.get_module(luna.RenderModule)
game_module: 'luna.GameModule' = luna.get_module(luna.GameModule)
platform_module: 'luna.PlatformModule' = luna.get_module(luna.PlatformModule)

if not __import__:
    from editor.scene.scene_window import SceneWindow


def update_asset(path, asset_type):
    asset = asset_module.load_asset(path, asset_type)
    asset_module.save_asset(asset, path)


class EditorModule(luna.LModule):
    main_scene_window: 'SceneWindow'
    _instance = None

    def __init__(self):
        super().__init__()

        self.project_dir = luna.get_config("DefaultProject")
        self.default_scene_path = luna.get_config("DefaultScene")
        self.main_scene_window = None
        self.main_side_bar = None
        self.now = time.time()
        self.reload_module = set()
        self.windows = {}

        self.current_window = None

        if False:
            update_asset("/assets/test.scn", luna.Scene)
            update_asset("/assets/built-in/Skybox/Skybox.mat", luna.MaterialTemplateAsset)
            update_asset("/assets/built-in/Skybox/Skybox.cubemap", luna.TextureCube)
            update_asset("/assets/built-in/Pbr.mat", luna.MaterialTemplateAsset)

    def get_window(self, cls):
        if cls in self.windows:
            return self.windows[cls]
        self.windows[cls] = cls()
        return self.windows[cls]

    def set_window(self, cls):
        if isinstance(cls, type):
            self.get_window(cls)
            if self.current_window:
                self.current_window.on_deactivate()
            self.current_window = self.windows[cls]
            self.current_window.on_activate()
        else:
            if self.current_window:
                self.current_window.on_deactivate()
            self.current_window = self.windows[cls.__class__]
            self.current_window.on_activate()

    def on_shutdown(self):
        luna.set_config("DefaultProject", self.project_dir)
        if self.main_scene_window.main_scene:
            luna.set_config("DefaultScene", self.main_scene_window.main_scene.path)

    def on_init(self):

        from editor.scene.scene_window import SceneWindow
        from editor.scene.scene_window import generate_doc_for_module

        global asset_module, game_module, render_module, platform_module
        from editor.ui.main_side_bar import SideTabBar
        self.main_side_bar = SideTabBar.instance()
        self.main_scene_window = self.get_window(SceneWindow)
        self.set_window(SceneWindow)

        generate_doc_for_module(luna)

        if not self.project_dir:
            self.main_scene_window.show_message_box("提示", "请点击文件打开项目")
        elif not self.default_scene_path:
            self.main_scene_window.show_message_box("提示", "请点击文件打开默认场景")

        if self.default_scene_path and self.project_dir:
            scn = asset_module.load_asset(self.default_scene_path, luna.Scene)
            self.main_scene_window.set_main_scene(scn)

    def on_load(self):
        pass

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

        main_view_offset = imgui.get_viewport_pos(self.main_scene_window.view_port)
        imgui.set_color(imgui.ImGuiCol_FrameBgActive, 0x4296FA59)
        window_module = luna.get_module(luna.PlatformModule)
        main_window = window_module.main_window
        size = luna.LVector2f(main_window.width, main_window.height)
        toolbar_color = imgui.get_style_color(imgui.ImGuiCol_TitleBg)
        imgui.push_style_color(imgui.ImGuiCol_WindowBg, toolbar_color)
        imgui.set_next_window_viewport(self.main_scene_window.view_port)
        imgui.set_next_window_pos(main_view_offset + luna.LVector2f(0, 70))
        imgui.set_next_window_size(luna.LVector2f(50, main_window.height))
        imgui.push_style_vec2(imgui.ImGuiStyleVar_FramePadding, luna.LVector2f(0.0, 0))
        imgui.push_style_vec2(imgui.ImGuiStyleVar_WindowPadding, luna.LVector2f(0.0, 0))
        imgui.push_style_vec2(imgui.ImGuiStyleVar_ItemSpacing, luna.LVector2f(0, ))
        imgui.push_style_color(imgui.ImGuiCol_Button, luna.LVector4f(0, 0, 0, 0))
        imgui.begin("Toolbar", imgui.ImGuiWindowFlags_NoSavedSettings |
                    imgui.ImGuiWindowFlags_NoResize | imgui.ImGuiWindowFlags_NoMove
                    | imgui.ImGuiWindowFlags_NoCollapse | imgui.ImGuiWindowFlags_NoTitleBar)

        self.main_side_bar.do_imgui(delta)
        imgui.pop_style_var(3)
        imgui.pop_style_color(2)
        imgui.end()

        imgui.set_next_window_size(size, 0)

        if self.current_window:
            self.current_window.do_imgui(delta)

        self.now = now

    @staticmethod
    def instance() -> 'EditorModule':
        if EditorModule._instance is None:
            EditorModule._instance = EditorModule()

        return EditorModule._instance
