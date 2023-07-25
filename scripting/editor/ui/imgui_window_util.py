import luna
from luna import LunaCore
from luna import imgui
def GenerateWindowStyle(width,height,offsetx,offsety):
    imgui.set_color(imgui.ImGuiCol_FrameBgActive, 0x4296FA59)
    size = luna.LVector2f(width, height)
    toolbar_color = imgui.get_style_color(imgui.ImGuiCol_TitleBg)
    imgui.push_style_color(imgui.ImGuiCol_WindowBg, toolbar_color)
    #imgui.set_next_window_viewport(self.view_port)
    #imgui.set_next_window_pos(luna.LVector2f(offsetx, offsety))
    #imgui.set_next_window_size(size, 0)
    #imgui.push_style_vec2(imgui.ImGuiStyleVar_WindowPadding, luna.LVector2f(0.0, 0))
    #imgui.push_style_vec2(imgui.ImGuiStyleVar_ItemSpacing, luna.LVector2f(5,0))
    imgui.push_style_vec2(imgui.ImGuiStyleVar_FramePadding, luna.LVector2f(16, 8))
    imgui.push_style_color(imgui.ImGuiCol_Button, luna.LVector4f(0, 0, 0, 0))

def PopWindowStyle():
    imgui.pop_style_var(1)
    imgui.pop_style_color(2)