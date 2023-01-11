import os
import luna
from core.editor_module import EditorModule
from luna import imgui
from ui.panel import PanelBase


class HierarchyPanel(PanelBase):
    def __init__(self) -> None:
        super(HierarchyPanel, self).__init__()
        self.scene = None
        self.title = "场景树"

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self) -> None:
        # super(PyHierarchyEditor, self).on_imgui()
        if not self.scene:
            return

        count = self.scene.get_entity_count()
        to_destroy_entity = set()
        for idx in range(0, count):
            entity: luna.Entity = self.scene.get_entity_at(idx)
            flag = 1 << 8 | 1 << 5

            def on_tree_node(hovered, held):
                self.on_entity_clicked(hovered, held, entity)

            if luna.imgui.tree_node_callback(id(entity), flag, on_tree_node):
                luna.imgui.text(entity.name)
                luna.imgui.tree_pop()

            luna.imgui.push_id(id(entity))
            if imgui.begin_popup_context_item(0, imgui.ImGuiPopupFlags_MouseButtonRight):
                if imgui.menu_item("删除"):

                    to_destroy_entity.add(entity)

                imgui.end_popup()
            luna.imgui.pop_id()

        for entity in to_destroy_entity:
            entity.owner_scene.destroy_entity(entity)

    def on_entity_clicked(self, hovered, held, entity):
        if held and entity:
            from core.editor import create_editor
            editor = create_editor(entity)
            EditorModule.instance().inspector_panel.set_editor(editor)
        pass
