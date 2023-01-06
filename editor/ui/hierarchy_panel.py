import os
import luna
from core.editor_core import EditorCore
from luna import editor


class PyHierarchyPanel(editor.PanelBase):
    def __init__(self) -> None:
        super(PyHierarchyPanel, self).__init__()
        self.scene = None
        self.title = "场景树"

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self) -> None:
        # super(PyHierarchyEditor, self).on_imgui()
        if not self.scene:
            return

        count = self.scene.get_entity_count()
        for idx in range(0, count):
            entity: luna.Entity = self.scene.get_entity_at(idx)
            flag = 1 << 8 | 1 << 5

            def on_tree_node(hovered, held):
                self.on_entity_clicked(hovered, held, entity)

            if luna.imgui.tree_node_callback(id(entity), flag, on_tree_node):
                luna.imgui.text(entity.name)
                luna.imgui.tree_pop()

    def on_entity_clicked(self, hovered, held, entity):
        if held and entity:
            from core.editor import create_editor
            editor = create_editor(entity)
            EditorCore.instance().inspector_panel.set_editor(editor)
        pass
