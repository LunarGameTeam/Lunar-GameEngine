import luna
from editor.core.editor_module import asset_module
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
        self.scene_icon = asset_module.load_asset("assets/built-in/Editor/Scene.png", luna.Texture2D)
        self.material_icon = asset_module.load_asset("assets/built-in/Editor/Material.png", luna.Texture2D)

    def do_imgui(self, delta_time):
        imgui.image_button("##Scene", self.scene_icon.get_rhi_texture(), luna.LVector2f(50, 50),
                           luna.LVector2f(0, 0), luna.LVector2f(1, 1),
                           luna.LVector4f(1.0, 1.0, 1.0, 0.1),
                           luna.LVector4f(1.0, 1.0, 1.0, 1.0))
        imgui.image_button("##Material", self.material_icon.get_rhi_texture(), luna.LVector2f(50, 50),
                           luna.LVector2f(0, 0), luna.LVector2f(1, 1),
                           luna.LVector4f(1.0, 1.0, 1.0, 0.0),
                           luna.LVector4f(0.7, 0.7, 0.7, 1.0))
