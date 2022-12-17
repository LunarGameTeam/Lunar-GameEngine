import os
import luna
from core.editor_core import EditorCore
from luna import editor


class PyHierarchyEditor(editor.HierarchyEditor):
    def __init__(self) -> None:
        super(PyHierarchyEditor, self).__init__()
        self.scene = None

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self) -> None:
        # super(PyHierarchyEditor, self).on_imgui()
        if not self.scene:
            return

        count = self.scene.get_entity_count()
        for idx in range(0, count):
            entity: luna.Entity = self.scene.get_entity_at(idx)
            id = entity.__repr__
            flag = 1 << 8 | 1 << 5

            def item_clicked(hovered, held):
                self.on_entity_clicked(hovered, held, entity)

            if self.custom_treenode(entity.name, flag, item_clicked):
                luna.imgui.text(entity.name)
                luna.imgui.tree_pop()

    def on_entity_clicked(self, hovered, held, entity):
        if held:
            EditorCore.instance().inspector_panel.set_selected(entity)
        pass
