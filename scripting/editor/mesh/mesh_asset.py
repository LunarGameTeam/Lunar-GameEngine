from editor.core.asset import register_asset_type, AssetType
from luna import imgui
from editor.mesh.model_window import ModelEditWindow

@register_asset_type
class MeshAssetType(AssetType):
    suffix = [".lmesh"]
    icon = imgui.ICON_FA_CIRCLE

    @staticmethod
    def on_double_click(cur_item):
        from editor.core.editor_module import EditorModule
        EditorModule.instance().add_custom_window(ModelEditWindow())


