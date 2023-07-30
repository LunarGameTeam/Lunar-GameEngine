from editor.core.asset import register_asset_type, AssetType
from luna import imgui


@register_asset_type
class SceneAssetType(AssetType):
    suffix = ".scn"
    icon = imgui.ICON_FA_TREE

    @staticmethod
    def on_double_click(cur_item):
        import luna
        from editor.core.editor_module import asset_module
        scn = asset_module.load_asset(cur_item.path, luna.Scene)
        from editor.core.editor_module import EditorModule
        from editor.scene.scene_window import SceneWindow

        scn_window = EditorModule.instance().get_window(SceneWindow)
        EditorModule.instance().set_window(scn_window)
        if scn:
            scn_window.set_main_scene(scn)
