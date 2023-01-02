import inspect
import os
import tkinter.filedialog
import types

import luna
from core.editor_core import EditorCore, asset_module, scene_module
from luna import editor
from luna import imgui
from core.editor_core import platform_module


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


class PyMainPanel(editor.MainPanel):
	def __init__(self) -> None:
		super(PyMainPanel, self).__init__()
		project_dir = luna.get_config("DefaultProject")
		default_scene = luna.get_config("DefaultScene")
		if default_scene and project_dir:
			scn = asset_module.load_asset(default_scene, luna.Scene)
			self.set_main_scene(scn)

	def set_main_scene(self, scn):
		if scn:
			entity = scn.find_entity("MainCamera")
			camera = entity.get_component(luna.CameraComponent)
			scene_module.add_scene(scn)
			EditorCore.instance().hierarchy_panel.set_scene(scn)

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
					name = tkinter.filedialog.askopenfilename(filetypes= (("scene files", "*.scn"),), initialdir=platform_module.project_dir)
					if name:
						engine_path = os.path.relpath(name, platform_module.project_dir)
						luna.set_config("DefaultScene", engine_path)
						scn = asset_module.load_asset(engine_path, luna.Scene)
						self.set_main_scene(scn)
				if imgui.menu_item("生成 Python API"):
					generate_doc_for_module(luna)
				if imgui.menu_item("退出"):
					exit(0)
				imgui.end_menu()
			imgui.end_menu_bar()
