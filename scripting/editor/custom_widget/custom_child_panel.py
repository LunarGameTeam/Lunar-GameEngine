import luna
from luna import LunaCore
from luna import imgui
class CustomChildPanelBase(object):
    def __init__(self,parent_window):
        self.title = "Panel"
        self.width = 1
        self.hight = 1
        self.has_menu = True
        self.parent_window = parent_window

        self.window_size = luna.LVector2f.zero()
        self.window_pos = luna.LVector2f.zero()
        self.flag = luna.imgui.ImGuiWindowFlags_NoCollapse | imgui.ImGuiWindowFlags_NoBringToFrontOnFocus

    def on_imgui(self, delta_time):
        vmax = imgui.get_window_content_max()
        vmin = imgui.get_window_content_min()
        self.width = vmax.x - vmin.x
        self.hight = vmax.y - vmin.y

    def do_imgui(self, delta_time):
        flag = self.flag
        if self.has_menu:
            flag = flag | luna.imgui.ImGuiWindowFlags_MenuBar
        imgui.begin(self.title + "###" + self.title + self.parent_window.window_name, flag, False)
        self.window_pos = imgui.get_window_pos()
        self.window_size = imgui.get_window_size()
        imgui.push_id(imgui.get_id(self.parent_window.window_name))
        self.on_imgui(delta_time)
        imgui.pop_id()
        luna.imgui.end()