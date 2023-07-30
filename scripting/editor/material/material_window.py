import luna
from editor.core.editor_module import EditorModule, asset_module
from editor.core.template_scene import TemplateSceneView
from editor.ui.panel import WindowBase, PanelBase
from luna import imgui


class MaterialViewPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.current_material_view = None
        self.loaded_materials = {}
        self.template_scene = asset_module.load_asset("/assets/built-in/Templates/MaterialScene.scn", luna.Scene)

    def set_material(self, material):
        material_view = self.loaded_materials.get(material, None)
        if not material_view:
            self.loaded_materials[material] = TemplateSceneView(self.template_scene)
        self.current_material_view = self.loaded_materials[material]

    def on_imgui(self, delta_time):
        super().on_imgui(delta_time)

        content = luna.imgui.get_content_region_avail()

        if self.current_material_view:
            luna.imgui.image(self.current_material_view.scene_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))


class MaterialWindow(WindowBase):
    main_scene: 'luna.Scene' = None
    window_name = "Scene Window"

    def __init__(self) -> None:
        super().__init__()

        from editor.ui.inspector_panel import InspectorPanel
        from editor.ui.library_panel import LibraryPanel

        self.library_panel = self.create_panel(LibraryPanel)
        self.inspector = self.create_panel(InspectorPanel)
        self.material_view = self.create_panel(MaterialViewPanel)

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
