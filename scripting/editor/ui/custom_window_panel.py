import luna
from luna import LunaCore
from luna import imgui

class CustomWindowPanel(object):
    def __init__(self,window_name,initwidth,initheight):
        super().__init__()
        self.width = initwidth
        self.height = initheight
        self.window_name = window_name
        self.title = window_name
        from editor.ui.panel import PanelBase
        self.panel_list = []

    def show_status(self, msg):
        self._status_time = 0
        self._status_open = True
        self._status_msg = msg

    def on_title(self):
        self.title = self.title

    def do_imgui(self, delta_time):
        from editor.ui.imgui_window_util import GenerateWindowStyle
        from editor.ui.imgui_window_util import PopWindowStyle
        GenerateWindowStyle(self.width,self.height,500,500)

        flags = imgui.ImGuiWindowFlags_NoCollapse \
                | imgui.ImGuiWindowFlags_NoMove \
                | imgui.ImGuiWindowFlags_NoBringToFrontOnFocus | luna.imgui.ImGuiWindowFlags_MenuBar

        dock_id = imgui.get_id(self.window_name)

        exiting = not imgui.begin(self.title + "###" + self.window_name, flags, True)

        imgui.set_cursor_pos(luna.LVector2f(50, 70))
        imgui.dock_space(dock_id, luna.LVector2f(0, 0), imgui.ImGuiDockNodeFlags_PassthruCentralNode)
        self.on_imgui(delta_time)

        PopWindowStyle()
        for editor in self.panel_list:
            imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
            editor.do_imgui(delta_time)
        imgui.end()
        if exiting:
            del self

    def on_imgui(self, delta_time):
        vmax = imgui.get_window_content_max()
        vmin = imgui.get_window_content_min()