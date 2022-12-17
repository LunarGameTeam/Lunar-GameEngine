from luna import *
import luna


asset_module: 'luna.AssetModule' = None
render_module: 'luna.RenderModule' = None
scene_module: 'luna.SceneModule' = None


class EditorCore(object):
    _instance = None

    def __init__(self):
        super(EditorCore, self).__init__()

        from ui.hierarchy_editor import PyHierarchyEditor
        from ui.main_editor import PyMainEditor
        from ui.inspector import PyInspectorEditor
        from ui.scene_editor import PySceneEditor
        from ui.library_editor import PyLibraryEditor

        self.hierarchy_editor = PyHierarchyEditor()
        self.main_editor = PyMainEditor()
        self.scene_editor = PySceneEditor()
        self.library_editor = PyLibraryEditor()
        self.inspector_panel = PyInspectorEditor()
        editor_module = self.editor_module = luna.get_module(luna.editor.EditorModule)

        editor_module.register_editor(self.main_editor)
        editor_module.register_editor(self.hierarchy_editor)
        editor_module.register_editor(self.scene_editor)
        editor_module.register_editor(self.library_editor)
        editor_module.register_editor(self.inspector_panel)

        global asset_module, scene_module, render_module
        asset_module = luna.get_module(luna.AssetModule)
        render_module = luna.get_module(luna.RenderModule)
        scene_module = luna.get_module(luna.SceneModule)
        self.init()

    def init(self):
        scn = asset_module.load_asset("/assets/test.scn", luna.Scene)
        entity = scn.find_entity("MainCamera")
        camera = entity.get_component(luna.CameraComponent)
        scene_module.add_scene(scn)
        self.hierarchy_editor.set_scene(scn)

    @staticmethod
    def instance() -> 'EditorCore':
        if EditorCore._instance is None:
            EditorCore._instance = EditorCore()
        return EditorCore._instance

