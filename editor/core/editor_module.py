from luna import *
import luna

asset_module: 'luna.AssetModule' = luna.get_module(luna.AssetModule)
render_module: 'luna.RenderModule' = luna.get_module(luna.RenderModule)
scene_module: 'luna.SceneModule' = luna.get_module(luna.SceneModule)
platform_module: 'luna.PlatformModule' = luna.get_module(luna.PlatformModule)


def update_asset(path, asset_type):
	asset = asset_module.load_asset(path, asset_type)
	asset_module.save_asset(asset, path)


class EditorModule(luna.LModule):
	_instance = None

	def __init__(self):
		super(EditorModule, self).__init__()
		self.editor_list = []
		if False:
			update_asset("/assets/test.scn", luna.Scene)
			update_asset("/assets/built-in/skybox/skybox.mat", luna.MaterialTemplateAsset)
			update_asset("/assets/built-in/skybox/sky_box.cubemap", luna.TextureCube)
			update_asset("/assets/built-in/pbr.mat", luna.MaterialTemplateAsset)

	def on_load(self):
		pass

	def on_init(self):

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

		self.editor_list.append(self.main_panel)
		self.editor_list.append(self.hierarchy_panel)
		self.editor_list.append(self.scene_panel)
		self.editor_list.append(self.library_panel)
		self.editor_list.append(self.inspector_panel)
		self.editor_list.append(self.demo_panel)

		generate_doc_for_module(luna)

		project_dir = luna.get_config("DefaultProject")
		default_scene = luna.get_config("DefaultScene")
		if default_scene and project_dir:
			scn = asset_module.load_asset(default_scene, luna.Scene)
			self.main_panel.set_main_scene(scn)

	def on_tick(self, delta_time):
		pass

	def on_imgui(self):
		for e in self.editor_list:
			e.do_imgui()

	@staticmethod
	def instance() -> 'EditorModule':
		if EditorModule._instance is None:
			EditorModule._instance = EditorModule()

		return EditorModule._instance
