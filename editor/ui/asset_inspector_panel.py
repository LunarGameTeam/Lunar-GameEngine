import core
import luna
from core.asset import FileInfo
from core.editor import create_editor, EditorBase, register_type_editor
from core.editor_module import asset_module
from luna import imgui
from ui.panel import PanelBase


@register_type_editor
class MeshRendererEditor(EditorBase):
    target_type = luna.MaterialTemplateAsset
    target: 'luna.MaterialTemplateAsset' = None

    def __init__(self, target):
        super().__init__(target)

    def get_indent(self):
        indent = super().get_indent()
        indent = max(indent, imgui.calc_text_size("Material").x)
        return indent

    def on_imgui(self):
        if self.on_category():
            params = self.target.params
            for it in params:
                a = it.value
                pass
            imgui.tree_pop()


class AssetPanel(PanelBase):
    item: FileInfo
    editor_list: list[core.editor.EditorBase]
    editor: core.editor.EditorBase
    selected_entity: luna.Entity
    world_sys: luna.GameModule

    def __init__(self) -> None:
        super().__init__()
        self.title = "AssetInspector"
        self.item = None
        self.editor = None
        self.width = 50

    def set_item(self, item: FileInfo):
        self.item = item
        if self.item:
            if self.item.name.endswith(".mat"):
                material = asset_module.load_asset(item.path, luna.MaterialTemplateAsset)
                editor = create_editor(material)
                self.editor = editor
        else:
            self.editor = None

    def imgui_menu(self):
        pass

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        self.imgui_menu()
        if self.item:
            imgui.text(self.item.path)
            if self.editor:
                self.editor.on_imgui()
        else:
            imgui.text("双击选中资源进行编辑")
        return
