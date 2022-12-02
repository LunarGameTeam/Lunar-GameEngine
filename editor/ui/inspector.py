import luna
from luna import editor, LModule


class PyInspectorEditor(editor.InspectorEditor):

	world_sys: luna.SceneModule

	def __init__(self) -> None:
		super(PyInspectorEditor, self).__init__()

	def on_tree_item(self, hovered, clicked):
		luna.imgui.text("123")

	def on_imgui(self) -> None:
		super(PyInspectorEditor, self).on_imgui()
		if self.custom_treenode("123", 0, self.on_tree_item):
			luna.imgui.tree_pop()

