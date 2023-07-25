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
        self.closed = False

        from editor.ui.custom_scene_view_pannel import CustomSceneViewPanel
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

    def do_imgui(self, delta_time):
        from editor.ui.imgui_window_util import GenerateWindowStyle
        from editor.ui.imgui_window_util import PopWindowStyle

        GenerateWindowStyle(self.width,self.height,500,500)

        flags = imgui.ImGuiWindowFlags_NoCollapse | luna.imgui.ImGuiWindowFlags_MenuBar
        dock_id = imgui.get_id(self.window_name)
        imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
        exiting = not imgui.begin(self.title + "###" + self.window_name, flags, True)
        imgui.dock_space(dock_id,luna.LVector2f(self.width,self.height),luna.imgui.ImGuiDockNodeFlags_PassthruCentralNode)
        imgui.set_cursor_pos(luna.LVector2f(50, 70))
        self.on_imgui(delta_time)

        PopWindowStyle()

        for editor in self.panel_list:
            #imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
            editor.do_imgui(delta_time)
        imgui.end()
        if exiting:
            self.closed = True

    def on_imgui(self, delta_time):
        pass