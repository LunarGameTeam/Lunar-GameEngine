import luna
from luna import LVector4f, LVector2f
from editor.core.editor_module import asset_module, EditorModule
from luna import imgui


class SideTabBar(object):
    _instance = None

    @staticmethod
    def instance() -> 'SideTabBar':
        if SideTabBar._instance is not None:
            return SideTabBar._instance
        SideTabBar._instance = SideTabBar()
        return SideTabBar._instance

    def __init__(self) -> None:
        super().__init__()
        self.title = "Toolbar"
        self.item = None
        self.editor = None
        from editor.scene.scene_window import SceneWindow

        from editor.material.material_window import MaterialWindow
        self.selected_tab = SceneWindow

        self.tabs = {
            SceneWindow: {
                "name": "##Scene",
                "icon": asset_module.load_asset("assets/built-in/Editor/Scene.png", luna.Texture2D),
            },
            MaterialWindow: {
                "name": "##Material",
                "icon": asset_module.load_asset("assets/built-in/Editor/Material.png", luna.Texture2D),
            }
        }

    def do_imgui(self, delta_time):
        for window_cls, data in self.tabs.items():
            if window_cls == self.selected_tab:
                bg_color = LVector4f(0.129, 0.588, 0.953, 1)
                hint_color = LVector4f(1.0, 1.0, 1.0, 1.0)

            else:
                bg_color = LVector4f(1.0, 1.0, 1.0, 0.0)
                hint_color = LVector4f(0.7, 0.7, 0.7, 1.0)
            if imgui.image_button(data.get("name"), data.get("icon").get_rhi_texture(), luna.LVector2f(50, 50),
                               LVector2f(0, 0), LVector2f(1, 1),
                               bg_color,
                               hint_color):
                self.selected_tab = window_cls
                EditorModule.instance().set_window(window_cls)