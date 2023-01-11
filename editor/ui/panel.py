import luna


class PanelBase(object):
	def __init__(self):
		self.title = "Panel"
		self.width = 100
		self.height = 100

	def on_imgui(self):
		pass

	def do_imgui(self):
		luna.imgui.begin(self.title, luna.imgui.ImGuiWindowFlags_NoCollapse)
		self.on_imgui()
		luna.imgui.end()