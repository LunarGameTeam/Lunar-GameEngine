import luna
from core.editor_module import EditorModule
from luna import imgui
from ui.panel import PanelBase


class HierarchyPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.scene = None
        self.title = "场景树"
        self.selected_entity = None

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self, delta_time) -> None:
        # super(PyHierarchyEditor, self).on_imgui()
        if not self.scene:
            return

        count = self.scene.get_entity_count()
        to_destroy_entity = set()
        for idx in range(0, count):
            entity: luna.Entity = self.scene.get_entity_at(idx)
            flag = 1 << 8 | 1 << 5

            clicked, expand = imgui.tree_node_callback(id(entity), flag)
            imgui.text("{} {}".format(imgui.ICON_FA_CUBE, entity.name))
            if expand:
                imgui.tree_pop()
            if clicked:
                self.on_entity_clicked(entity)

            luna.imgui.push_id(id(entity))
            if imgui.begin_popup_context_item(0, imgui.ImGuiPopupFlags_MouseButtonRight):
                if imgui.menu_item("删除"):
                    to_destroy_entity.add(entity)
                imgui.end_popup()
            luna.imgui.pop_id()

        for entity in to_destroy_entity:
            entity.owner_scene.destroy_entity(entity)

    def on_entity_clicked(self, entity):
        if entity:
            self.selected_entity = entity
            from core.editor import create_editor
            editor = create_editor(entity)
            from ui.inspector_panel import InspectorPanel
            self.parent_window.get_panel(InspectorPanel).set_editor(editor)
        pass
