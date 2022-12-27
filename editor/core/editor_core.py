from luna import *
import luna

asset_module: 'luna.AssetModule' = None
render_module: 'luna.RenderModule' = None
scene_module: 'luna.SceneModule' = None
platform_module: 'luna.PlatformModule' = None


class EditorCore(object):
    _instance = None

    def __init__(self):
        super(EditorCore, self).__init__()

        from ui.hierarchy_panel import PyHierarchyPanel
        from ui.main_panel import PyMainPanel
        from ui.inspector_panel import PyInspectorPanel
        from ui.scene_panel import PyScenePanel
        from ui.library_panel import PyLibraryPanel
        from ui.imgui_demo import DemoPanel

        global asset_module, scene_module, render_module, platform_module
        asset_module = luna.get_module(luna.AssetModule)
        render_module = luna.get_module(luna.RenderModule)
        scene_module = luna.get_module(luna.SceneModule)
        platform_module = luna.get_module(luna.PlatformModule)

        self.hierarchy_panel = PyHierarchyPanel()
        self.demo_panel = DemoPanel()
        self.main_panel = PyMainPanel()
        self.scene_panel = PyScenePanel()
        self.library_panel = PyLibraryPanel()
        self.inspector_panel = PyInspectorPanel()
        editor_module = self.editor_module = luna.get_module(luna.editor.EditorModule)

        editor_module.register_panel(self.main_panel)
        editor_module.register_panel(self.hierarchy_panel)
        editor_module.register_panel(self.scene_panel)
        editor_module.register_panel(self.library_panel)
        editor_module.register_panel(self.inspector_panel)
        editor_module.register_panel(self.demo_panel)

        self.init()

    def init(self):
        scn = asset_module.load_asset("/assets/test.scn", luna.Scene)
        entity = scn.find_entity("MainCamera")
        camera = entity.get_component(luna.CameraComponent)
        scene_module.add_scene(scn)
        self.hierarchy_panel.set_scene(scn)

    @staticmethod
    def instance() -> 'EditorCore':
        if EditorCore._instance is None:
            EditorCore._instance = EditorCore()
        return EditorCore._instance

