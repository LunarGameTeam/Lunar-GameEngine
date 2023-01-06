from luna import *
import luna

asset_module: 'luna.AssetModule' = luna.get_module(luna.AssetModule)
render_module: 'luna.RenderModule' = luna.get_module(luna.RenderModule)
scene_module: 'luna.SceneModule' = luna.get_module(luna.SceneModule)
platform_module: 'luna.PlatformModule' = luna.get_module(luna.PlatformModule)


def update_asset(path, asset_type):
    asset = asset_module.load_asset(path, asset_type)
    asset_module.save_asset(asset, path)


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
        from ui.main_panel import generate_doc_for_module

        global asset_module, scene_module, render_module, platform_module

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

        if False:
            generate_doc_for_module(luna)
            update_asset("/assets/test.scn", luna.Scene)
            update_asset("/assets/built-in/skybox/skybox.mat", luna.MaterialTemplateAsset)
            update_asset("/assets/built-in/skybox/sky_box.cubemap", luna.TextureCube)
            update_asset("/assets/built-in/pbr.mat", luna.MaterialTemplateAsset)

    def init(self):

        project_dir = luna.get_config("DefaultProject")
        default_scene = luna.get_config("DefaultScene")
        if default_scene and project_dir:
            scn = asset_module.load_asset(default_scene, luna.Scene)
            self.main_panel.set_main_scene(scn)

    @staticmethod
    def instance() -> 'EditorCore':
        if EditorCore._instance is None:
            EditorCore._instance = EditorCore()
            EditorCore._instance.init()

        return EditorCore._instance

