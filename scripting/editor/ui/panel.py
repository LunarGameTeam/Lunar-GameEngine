import os
import tkinter
import typing
from typing import T

import luna
from editor.core.editor_module import platform_module
from luna import LunaCore
from luna import imgui


class WindowBase(object):
    panel_list: 'list[PanelBase]'
    id = 1
    window_name = "MainWindow"

    def __init__(self):
        super().__init__()
        self.width = 1
        self.height = 1
        self.title = "###Luna Editor"
        self.id = WindowBase.id

        if self.id == 1:
            self.view_port = imgui.get_main_viewport_id()
        else:
            self.view_port = imgui.get_id(self.window_name)

        self.panel_list = []

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

    def create_panel(self, panel_type: 'typing.Type[T]') -> 'T':
        panel = panel_type()
        self.panel_list.append(panel)
        panel.parent_window = self
        return panel

    def add_panel(self, panel):
        self.panel_list.append(panel)
        panel.parent_window = self
        return panel

    def get_panel(self, panel_tye: 'typing.Type[T]') -> 'T':
        for editor in self.panel_list:
            if isinstance(editor, panel_tye):
                return editor
        return None
    def on_activate(self):
        pass

    def on_deactivate(self):
        pass

    def show_box(self, title, callback):
        pass

    def show_message_box(self, title, msg):
        self._dialog_title = title
        self._dialog_open = True
        self._dialog_msg = msg

    def on_simple_mb(self):
        imgui.set_next_window_size(self._dialog_min, 0)
        view_pos = self.view_pos
        x = self.view_pos.x + self.width / 2.0
        y = self.view_pos.y + self.height / 2.0
        imgui.set_next_window_pos(luna.LVector2f(x, y), imgui.ImGuiCond_Always, luna.LVector2f(0.5, 0.5))
        show, is_open = imgui.begin_popup_modal(self._dialog_title, self._dialog_open,
                                                imgui.ImGuiWindowFlags_NoSavedSettings)
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

    def on_toolbar_menu(self):
        if imgui.begin_menu("文件", True):
            if imgui.menu_item("打开项目"):
                name = tkinter.filedialog.askdirectory(initialdir=platform_module.engine_dir)
                if name:
                    platform_module.set_project_dir(name)

            if imgui.menu_item("导入资源"):
                name = tkinter.filedialog.askopenfilename(
                    filetypes=(("obj files", "*.obj"), ("fbx files", "*.fbx"), ("gltf files", "*.gltf"),),
                    initialdir=platform_module.project_dir + "/assets")
                file_with_extension = os.path.basename(name)
                (file_without_extension, file_extension) = os.path.splitext(file_with_extension)
                luna.editor.import_mesh(name, platform_module.project_dir + "/assets", file_without_extension,
                                        file_extension)

            imgui.end_menu()

        if imgui.begin_menu("工具", True):
            if imgui.menu_item("关于"):
                self.show_message_box("Luna Editor 0.1", "made by Isak Wong, Pancy Star")
            imgui.end_menu()

    def do_imgui(self, delta_time):
        self.on_title()
        main_view_offset = imgui.get_viewport_pos(self.view_port);
        exiting = False
        imgui.set_next_window_pos(main_view_offset + luna.LVector2f(0, 0), imgui.ImGuiCond_Always)
        imgui.push_style_vec2(imgui.ImGuiStyleVar_FramePadding, luna.LVector2f(16, 8))
        flags = imgui.ImGuiWindowFlags_NoCollapse \
                | imgui.ImGuiWindowFlags_NoMove \
                | imgui.ImGuiWindowFlags_NoBringToFrontOnFocus | luna.imgui.ImGuiWindowFlags_MenuBar
        imgui.set_next_window_viewport(self.view_port)
        dock_id = imgui.get_id(self.__class__.window_name)

        exiting = not imgui.begin("MainWindow", flags, True)

        imgui.set_cursor_pos(luna.LVector2f(50, 70))
        imgui.dock_space(dock_id, luna.LVector2f(0, 0), imgui.ImGuiDockNodeFlags_PassthruCentralNode)
        self.on_imgui(delta_time)
        imgui.pop_style_var(1)
        for editor in self.panel_list:
            imgui.set_next_window_viewport(self.view_port)
            imgui.set_next_window_dock_id(dock_id, imgui.ImGuiCond_FirstUseEver)
            editor.do_imgui(delta_time)

        self.handle_window_move()

        imgui.end()
        if exiting:
            LunaCore.instance().exit()

    def on_imgui(self, delta_time):
        vmax = imgui.get_window_content_max()
        vmin = imgui.get_window_content_min()
        self.width = imgui.get_window_size().x
        self.height = imgui.get_window_size().y

        if imgui.begin_menu_bar():
            self.on_toolbar_menu()
            imgui.end_menu_bar()

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
                        imgui.ImGuiWindowFlags_NoCollapse | imgui.ImGuiWindowFlags_NoSavedSettings)
            imgui.text(self._status_msg)
            imgui.end()
            imgui.pop_style_color(1)

        self.on_simple_mb()


class PanelBase(object):
    parent_window: 'WindowBase'

    _instance = None

    @classmethod
    def get_singleton(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.title = "Panel"
        self.width = 1
        self.height = 1
        self.has_menu = True
        self.parent_window = None

        self.window_size = luna.LVector2f.zero()
        self.window_pos = luna.LVector2f.zero()
        self.flag = luna.imgui.ImGuiWindowFlags_NoCollapse | imgui.ImGuiWindowFlags_NoBringToFrontOnFocus

    def on_imgui(self, delta_time):
        vmax = imgui.get_window_content_max()
        vmin = imgui.get_window_content_min()
        self.width = vmax.x - vmin.x
        self.height = vmax.y - vmin.y

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
