from editor.ui.panel import PanelBase
from luna import imgui


# 一个Scene 的 View 的UI Panel
class SceneViewPanel(PanelBase):

    def __init__(self):
        super().__init__()
        self.last_min = None
        self.last_max = None
        self.dragging = False
        self.need_update_texture = True
        self.current_scene_view = None

    def on_view_imgui(self, content):
        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()

        if imgui.is_mouse_dragging(0, -1.0):
            if vmin != self.last_min or vmax != self.last_max:
                self.dragging = True
            self.last_min = imgui.get_window_content_min()
            self.last_max = imgui.get_window_content_max()

        if self.need_update_texture and self.current_scene_view:
            if content.x > 0 and content.y > 0:
                self.current_scene_view.resize_scene_texture(content.x, content.y)
                self.need_update_texture = False

        if self.dragging and self.current_scene_view:
            if imgui.is_mouse_released(0):
                self.current_scene_view.resize_scene_texture(vmax.x - vmin.x, vmax.y - vmin.y)
                self.dragging = False


