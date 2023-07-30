from editor.core.asset import register_asset_type, AssetType
from luna import imgui


@register_asset_type
class MaterialAssetType(AssetType):
    suffix = [".mat"]
    icon = imgui.ICON_FA_CIRCLE

    @staticmethod
    def on_double_click(cur_item):
        from editor.ui.inspector_panel import InspectorPanel
        from editor.core.editor_module import asset_module
        from editor.core.inspector_base import create_inspector
        from editor.core.editor_module import EditorModule
        import luna
        material = asset_module.load_asset(cur_item.path, luna.MaterialTemplateAsset)
        editor = create_inspector(material)
        from editor.material.material_window import MaterialWindow

        material_window = EditorModule.instance().get_window(MaterialWindow)
        EditorModule.instance().set_window(material_window)
        from editor.material.material_window import MaterialViewPanel
        view_panel = material_window.get_panel(MaterialViewPanel)
        view_panel.set_material(material)

