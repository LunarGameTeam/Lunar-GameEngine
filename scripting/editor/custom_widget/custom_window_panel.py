import luna
from luna import LunaCore
from luna import imgui

class CustomWindowPanel(object):
    def __init__(self,window_name,initwidth,inithight):
        super().__init__()
        self.width = initwidth
        self.hight = inithight
        self.window_name = window_name
        self.title = window_name
        from editor.ui.panel import PanelBase
        self.panel_list = []
        self.closed = False

        from editor.custom_widget.custom_scene_view_pannel import CustomSceneViewPanel
        self.add_panel(CustomSceneViewPanel)
    def show_status(self, msg):
        self._status_time = 0
        self._status_open = True
        self._status_msg = msg

    def check_closed(self):
        return self.closed

    def add_panel(self, panel_type: 'typing.Type[T]') -> 'T':
        panel = panel_type()
        self.panel_list.append(panel)
        panel.parent_window = self
        return panel

    def on_title(self):
        self.title = self.title

    def GenerateWindowStyle(self):
        imgui.set_color(imgui.ImGuiCol_FrameBgActive, 0x4296FA59)
        toolbar_color = imgui.get_style_color(imgui.ImGuiCol_TitleBg)
        imgui.push_style_color(imgui.ImGuiCol_WindowBg, toolbar_color)
        imgui.push_style_vec2(imgui.ImGuiStyleVar_FramePadding, luna.LVector2f(16, 8))
        imgui.push_style_color(imgui.ImGuiCol_Button, luna.LVector4f(0, 0, 0, 0))

    def PopWindowStyle(self):
        imgui.pop_style_var(1)
        imgui.pop_style_color(2)

    def do_imgui(self, delta_time):
        self.GenerateWindowStyle()
        flags = imgui.ImGuiWindowFlags_NoCollapse | luna.imgui.ImGuiWindowFlags_MenuBar
        dock_id = imgui.get_id(self.window_name)
        imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
        exiting = not imgui.begin(self.title + "###" + self.window_name, flags, True)
        imgui.dock_space(dock_id,luna.LVector2f(0, 0),luna.imgui.ImGuiDockNodeFlags_PassthruCentralNode)
        imgui.set_cursor_pos(luna.LVector2f(50, 70))
        self.on_imgui(delta_time)

        self.PopWindowStyle()

        for editor in self.panel_list:
            #imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
            editor.do_imgui(delta_time)
        imgui.end()
        if exiting:
            self.closed = True

    def on_imgui(self, delta_time):
        pass