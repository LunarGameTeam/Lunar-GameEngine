import luna
from editor.core.editor_module import EditorModule, asset_module
from editor.core.template_scene import TemplateSceneView
from editor.ui.panel import WindowBase
from editor.ui.view_panel import SceneViewPanel
from luna import imgui


class MaterialAssetViewPanel(SceneViewPanel):
    def __init__(self) -> None:
        super().__init__()
        self.loaded_materials = {}  # type: dict[luna.MaterialTemplateAsset, TemplateSceneView]
        self.template_scene = asset_module.load_asset("/assets/built-in/TemplateScene/MaterialScene.scn", luna.Scene)
        self.need_update_texture = True
        self.dragging = False

    def set_material(self, material):
        material_view = self.loaded_materials.get(material, None)
        if not material_view:
            self.loaded_materials[material] = TemplateSceneView(self.template_scene)
        self.current_scene_view = self.loaded_materials[material]
        if material:
            self.current_scene_view.camera.transform.local_position = luna.LVector3f(0, 0, -5)
            material_entity = self.current_scene_view.find_entity("Material")
            material_entity.get_component(luna.StaticMeshRenderer).material = material

    def on_imgui(self, delta_time):
        super().on_imgui(delta_time)

        content = luna.imgui.get_content_region_avail()

        if self.current_scene_view:
            luna.imgui.image(self.current_scene_view.scene_texture, content, luna.LVector2f(0, 0),
                             luna.LVector2f(1, 1))
        self.on_view_imgui(content)


class MaterialWindow(WindowBase):
    main_scene: 'luna.Scene' = None
    window_name = "Scene Window"

    def __init__(self) -> None:
        super().__init__()

        from editor.ui.inspector_panel import InspectorPanel
        from editor.ui.library_panel import LibraryPanel

        self.library_panel = self.create_panel(LibraryPanel)
        self.inspector = self.create_panel(InspectorPanel)
        self.material_view = self.create_panel(MaterialAssetViewPanel)

    def on_title(self):
        proj_dir = EditorModule.instance().project_dir
        scene_path = EditorModule.instance().default_scene_path
        app_title = imgui.ICON_FA_MOON + "  Luna Editor"
        self.title = "{}\t\t{}\t\t{}".format(app_title, proj_dir, scene_path)

    def on_toolbar_menu(self):
        super().on_toolbar_menu()
        if imgui.begin_menu("文件", True):
            imgui.end_menu()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)

    def on_activate(self):
        if self.material_view.template_scene:
            self.material_view.template_scene.renderable = True

    def on_deactivate(self):
        if self.material_view.template_scene:
            self.material_view.template_scene.renderable = False
