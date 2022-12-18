import inspect
import os
import types

import luna
from luna import editor
from luna import imgui


class PyMainEditor(editor.MainEditor):
	def __init__(self) -> None:
		super(PyMainEditor, self).__init__()

	def generate_doc_for_module(self, mymodule: types.ModuleType) -> object:
		module_name = mymodule.__name__
		module_name = module_name.replace('.', '/')
		p = "temp/{0}".format(module_name)
		if not os.path.exists("temp"):
			os.mkdir("temp")
		if not os.path.exists(p):
			os.mkdir(p)

		header = "import luna\nimport typing\nT=typing.TypeVar(\"T\")"
		doc = mymodule.__doc__ + "\n"

		for name, obj in inspect.getmembers(mymodule):
			if inspect.ismodule(obj):
				my_module_name: str = obj.__name__
				my_module_name = my_module_name.split('.')[-1]
				header = header + "\nfrom luna import {0}\n".format(my_module_name)
				self.generate_doc_for_module(obj)

			if inspect.isclass(obj):
				doc = doc + obj.__doc__ + "\n\n\n"
		try:
			doc = header + doc + "\n\ncore : luna.LunaCore = None"
			f = open(p + "/__init__.py", "w", encoding='utf-8')
			f.write(doc)
			f.close()
		except Exception as err:
			pass
		return

	def on_imgui(self) -> None:
		super(PyMainEditor, self).on_imgui()
		if imgui.begin_menu_bar():
			if imgui.begin_menu("文件"):
				if imgui.menu_item("生成 Python API"):
					self.generate_doc_for_module(luna)
				imgui.end_menu()
			imgui.end_menu_bar()
