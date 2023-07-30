import luna
from luna import imgui
from editor.ui.panel import PanelBase


class HierarchyPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.scene = None
        self.title = "场景树"
        self.selected_entity = None

    def create_geometry(self, mesh_asset):
        if self.scene:
            entity = self.scene.create_entity("Cube")
            renderer = entity.add_component(luna.MeshRenderer)
            from editor.core.editor_module import asset_module
            renderer.mesh = asset_module.load_asset(mesh_asset, luna.MeshAsset)
            renderer.material = asset_module.load_asset("/assets/built-in/Pbr.mat", luna.MaterialTemplateAsset)

    def create_point_light(self):
        if self.scene:
            entity = self.scene.create_entity("PointLight")
            transform = entity.add_component(luna.Transform)
            light = entity.add_component(luna.PointLightComponent)
            light.color = luna.LVector4f(1, 1, 1, 1)
            light.intensity = 1.0

    def create_direction_light(self):
        if self.scene:
            entity = self.scene.create_entity("Main Direction Light")
            transform = entity.add_component(luna.Transform)
            light = entity.add_component(luna.DirectionLightComponent)
            light.color = luna.LVector4f(1, 1, 1, 1)
            light.intensity = 1.0

    def on_menu(self):
        if luna.imgui.begin_menu_bar():
            if luna.imgui.begin_menu("创建", True):
                if imgui.menu_item("Scene Environment"):
                    entity = self.scene.create_entity("SceneEnv")
                    scene_env = entity.add_component(luna.SceneEnvComponent)
                if imgui.menu_item("Cube"):
                    self.create_geometry("/assets/built-in/Geometry/Box.lmesh")
                if imgui.menu_item("Sphere"):
                    self.create_geometry("/assets/built-in/Geometry/Sphere.lmesh")
                if imgui.menu_item("Plane"):
                    self.create_geometry("/assets/built-in/Geometry/Plane.lmesh")
                if imgui.menu_item("PointLight"):
                    self.create_point_light()
                if imgui.menu_item("Directional Light"):
                    if self.scene:
                        entity = self.scene.create_entity("Main Directional Light")
                        transform = entity.add_component(luna.Transform)
                        light = entity.add_component(luna.DirectionLightComponent)
                        light.color = luna.LVector4f(1, 1, 1, 1)
                        light.intensity = 1.0
                    pass
                luna.imgui.end_menu()
            luna.imgui.end_menu_bar()

    def set_scene(self, scene):
        self.scene = scene

    def on_imgui(self, delta_time) -> None:
        # super(PyHierarchyEditor, self).on_imgui()
        if not self.scene:
            return
        self.on_menu()
        count = self.scene.get_entity_count()
        to_destroy_entity = set()
        for idx in range(0, count):
            entity: luna.Entity = self.scene.get_entity_at(idx)
            flag = 1 << 8 | 1 << 5

            clicked, expand, double_click = imgui.tree_node_callback(id(entity), luna.LVector2f(-1, 32), flag)
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
            from editor.core.inspector_base import create_inspector
            editor = create_inspector(entity)
            from editor.ui.inspector_panel import InspectorPanel
            self.parent_window.get_panel(InspectorPanel).set_editor(editor)
        pass
