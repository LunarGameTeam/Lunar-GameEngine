import typing
from typing import T

import luna
from luna import imgui


class WindowBase(object):
    editor_list: 'list[PanelBase]'
    id = 1
    window_name = "MainWindow"

    def __init__(self):
        super().__init__()
        self.width = 1
        self.height = 1
        self.title = "###Luna Editor"
        self.id = WindowBase.id
        WindowBase.id = WindowBase.id + 1

        if self.id == 1:
            self.view_port = imgui.get_main_viewport_id()
        else:
            self.view_port = imgui.get_id(self.window_name)

        self.editor_list = []

        self.view_pos = luna.LVector2f(0, 0)

        self._status_show_time = 0.5
        self._status_destroy_time = 4.0
        self._status_time = 0
        self._status_msg = ""
        self._status_open = False

        self._dialog_min = luna.LVector2f(400, 200)
        self._dialog_open = False
        self._dialog_msg = ""
        self._dialog_title = "Title"

        self.press_pos = None
        self.press_titlebar = None

    def add_panel(self, panel_type: 'typing.Type[T]') -> 'T':
        panel = panel_type()
        self.editor_list.append(panel)
        panel.parent_window = self
        return panel

    def get_panel(self, panel_tyee: 'typing.Type[T]') -> 'T':
        for editor in self.editor_list:
            if isinstance(editor, panel_tyee):
                return editor
        return None

    def show_box(self, title, callback):
        pass

    def show_message_box(self, title, msg):
        self._dialog_title = title
        self._dialog_open = True
        self._dialog_msg = msg

    def on_simple_mb(self):
        imgui.set_next_window_size(self._dialog_min, 0)
        view_pos = self.view_pos
        view_pos.x += self.width / 2.0
        view_pos.y += self.height / 2.0

        imgui.set_next_window_pos(view_pos, imgui.ImGuiCond_Always, luna.LVector2f(0.5, 0.5))
        show, is_open = imgui.begin_popup_modal(self._dialog_title, self._dialog_open, 0)
        if show:
            imgui.text(self._dialog_msg)
            imgui.end_popup()
        self._dialog_open = is_open
        if self._dialog_open:
            imgui.open_popup(self._dialog_title, 0)

    def show_status(self, msg):
        self._status_time = 0
        self._status_open = True
        self._status_msg = msg

    def on_title(self):
        self.title = self.title

    def setup_color(self):
        imgui.set_color(imgui.ImGuiCol_FrameBgActive, 0x4296FA59)

    def handle_window_move(self):
        window_module = luna.get_module(luna.PlatformModule)
        main_window = window_module.main_window
        mouse_pos = main_window.get_mouse_pos()
        if imgui.is_mouse_dragging(0, -1) and self.press_pos:
            if self.press_titlebar:
                delta_x = self.press_pos.x - mouse_pos.x
                delta_y = self.press_pos.y - mouse_pos.y
                new_x = main_window.get_window_x() - delta_x
                new_y = main_window.get_window_y() - delta_y

                main_window.set_window_pos(int(new_x), int(new_y))
                self.press_pos = mouse_pos

        if imgui.is_mouse_down(0):
            pos = imgui.get_mouse_pos()
            y = pos.y - imgui.get_window_pos().y
            if y < 20 and not self.press_titlebar:
                self.press_titlebar = True
                self.press_pos = main_window.get_mouse_pos()

        if imgui.is_mouse_released(0):
            if self.press_titlebar:
                self.press_titlebar = None

    def do_imgui(self, delta_time):
        self.setup_color()
        self.on_title()
        window_module = luna.get_module(luna.PlatformModule)
        main_window = window_module.main_window
        size = luna.LVector2f(main_window.width, main_window.height)
        imgui.set_next_window_size(size, 0)

        exiting = False

        imgui.push_style_vec2(imgui.ImGuiStyleVar_FramePadding, luna.LVector2f(16, 8))
        flags = imgui.ImGuiWindowFlags_MenuBar \
                | imgui.ImGuiWindowFlags_NoCollapse \
                | imgui.ImGuiWindowFlags_NoMove \
                | imgui.ImGuiWindowFlags_NoBringToFrontOnFocus
        imgui.set_next_window_viewport(self.view_port)
        imgui.set_next_window_pos(luna.LVector2f(0, 0), 0, luna.LVector2f(0, 0))

        dock_id = imgui.get_id(self.__class__.window_name)

        exiting = not imgui.begin(self.title + "###" + self.__class__.window_name, flags, True)

        imgui.dock_space(dock_id, luna.LVector2f(0, 0), imgui.ImGuiDockNodeFlags_PassthruCentralNode)
        self.on_imgui(delta_time)
        imgui.pop_style_var(1)
        for editor in self.editor_list:
            imgui.set_next_window_viewport(self.view_port)
            imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
            editor.do_imgui(delta_time)

        self.handle_window_move()

        imgui.end()
        if exiting:
            luna.exit()

    def on_imgui(self, delta_time):
        vmax = imgui.get_window_content_max()
        vmin = imgui.get_window_content_min()
        self.width = imgui.get_window_size().x
        self.height = imgui.get_window_size().y

        if self._status_open:
            self._status_time += delta_time
            width = 300
            height = 100
            pos_x = self.width - width + self.view_pos.x
            if self._status_time < self._status_show_time:
                height = height * self._status_time / self._status_show_time
            imgui.set_next_window_pos(luna.LVector2f(pos_x, self.view_pos.y + self.height), 0, luna.LVector2f(0, 1))
            imgui.set_next_window_size(luna.LVector2f(width, height), 0)
            if self._status_time > self._status_destroy_time:
                self._status_open = False

        if self._status_open:
            imgui.push_style_color(imgui.ImGuiCol_WindowBg, luna.LVector4f(0.08, 0.08, 0.09, 1.00))
            imgui.begin("Status",
                        imgui.ImGuiWindowFlags_NoCollapse)
            imgui.text(self._status_msg)
            imgui.end()
            imgui.pop_style_color(1)

        self.on_simple_mb()


class PanelBase(object):
    parent_window: 'WindowBase'

    def __init__(self):
        self.title = "Panel"
        self.width = 1
        self.height = 1
        self.has_menu = False
        self.parent_window = None

        self.window_size = luna.LVector2f.zero()
        self.window_pos = luna.LVector2f.zero()

    def on_imgui(self, delta_time):
        vmax = imgui.get_window_content_max()
        vmin = imgui.get_window_content_min()
        self.width = vmax.x - vmin.x
        self.height = vmax.y - vmin.y

    def do_imgui(self, delta_time):
        imgui.begin(self.title + "###" + self.title + self.parent_window.window_name,
                    luna.imgui.ImGuiWindowFlags_NoCollapse | luna.imgui.ImGuiWindowFlags_MenuBar,
                    False)
        self.window_pos = imgui.get_window_pos()
        self.window_size = imgui.get_window_size()
        imgui.push_id(imgui.get_id(self.parent_window.window_name))
        self.on_imgui(delta_time)
        imgui.pop_id()
        luna.imgui.end()
