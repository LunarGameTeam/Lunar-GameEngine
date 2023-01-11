import inspect
import os
import tkinter.filedialog
import types

import luna
from luna import editor
from luna import imgui
from core.editor_module import platform_module, EditorModule, asset_module, scene_module

from ui.panel import PanelBase


def generate_class_doc(cls: 'type'):
	cls_doc = cls.__doc__ + "\n\n\n"
	return cls_doc


def generate_doc_for_module(target: 'types.ModuleType') -> object:
	module_name = target.__name__
	module_name = module_name.replace('.', '/')
	p = "temp/{0}".format(module_name)
	if not os.path.exists("temp"):
		os.mkdir("temp")
	if not os.path.exists(p):
		os.mkdir(p)

	header = ""
	header += "import luna\n"
	header += "import typing\n"
	header += "from typing import *\n"
	header += "T=typing.TypeVar(\"T\")\n"
	header += "core : 'luna.LunaCore' = None\n"

	doc = target.__doc__
	for name, mem in inspect.getmembers(target):
		if inspect.ismodule(mem):
			my_module_name: str = mem.__name__
			my_module_name = my_module_name.split('.')[-1]
			header = header + "\nfrom luna import {0}\n".format(my_module_name)
			generate_doc_for_module(mem)
		elif inspect.isclass(mem):
			class_doc = generate_class_doc(mem)
			doc = doc + class_doc
	try:
		doc = header + doc
		f = open(p + "/__init__.py", "w", encoding='utf-8')
		f.write(doc)
		f.close()
	except OSError as err:
		assert False
	return


class PyMainPanel(PanelBase):
	main_scene: 'luna.Scene' = None

	def __init__(self) -> None:
		super(PyMainPanel, self).__init__()
		self.title = "Luna Editor"
		self.main_scene = None

	def set_main_scene(self, scn):
		if scn:
			self.main_scene = scn
			entity = scn.find_entity("MainCamera")
			camera = entity.get_component(luna.CameraComponent)
			scene_module.add_scene(scn)
			EditorModule.instance().hierarchy_panel.set_scene(scn)
			EditorModule.instance().scene_panel.set_scene(scn)

	def do_imgui(self):
		window_module = luna.get_module(luna.WindowModule)
		main_window = window_module.main_window
		size = luna.LVector2f(main_window.width, main_window.height)
		imgui.set_next_window_size(size, 0)
		imgui.set_next_window_pos(luna.LVector2f(0, 0), 0, luna.LVector2f(0, 0))
		imgui.begin(self.title, 0)
		self.on_imgui()
		imgui.dock_space(1, luna.LVector2f(0, 0), 0)
		imgui.end()

	def on_imgui(self) -> None:
		super(PyMainPanel, self).on_imgui()
		if imgui.begin_menu_bar():
			if imgui.begin_menu("文件", True):
				if imgui.menu_item("打开项目"):
					name = tkinter.filedialog.askdirectory(initialdir=platform_module.engine_dir)
					if name:
						platform_module.set_project_dir(name)
					pass
				if imgui.menu_item("打开场景"):
					name = tkinter.filedialog.askopenfilename(filetypes=(("scene files", "*.scn"),),
															  initialdir=platform_module.project_dir)
					if name:
						engine_path = os.path.relpath(name, platform_module.project_dir)
						luna.set_config("DefaultScene", engine_path)
						scn = asset_module.load_asset(engine_path, luna.Scene)
						self.set_main_scene(scn)
				if imgui.menu_item("生成 Python API"):
					generate_doc_for_module(luna)
				if imgui.menu_item("保存场景"):
					asset_module.save_asset(self.main_scene, self.main_scene.path)
				if imgui.menu_item("退出"):
					exit(0)
				imgui.end_menu()
			imgui.end_menu_bar()
