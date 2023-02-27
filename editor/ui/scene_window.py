import inspect
import os
import tkinter.filedialog
import types
import typing
from typing import T

import luna
from core.editor_module import platform_module, EditorModule, asset_module, game_module
from luna import imgui
from ui.panel import WindowBase


def generate_class_doc(cls: 'type'):
    cls_doc = cls.__doc__ + "\n\n\n"
    return cls_doc


def generate_doc_for_module(target: 'types.ModuleType') -> object:
    module_name = target.__name__
    module_name = module_name.replace('.', '/')
    p = "temp/{0}".format(module_name)
    if not os.path.exists("temp"):
        os.mkdir("temp")
    if not os.path.exists(p):
        os.mkdir(p)

    header = ""
    header += "import luna\n"
    header += "import typing\n"
    header += "from typing import *\n"
    header += "T=typing.TypeVar(\"T\")\n"
    header += "core : 'luna.LunaCore' = None\n"

    doc = target.__doc__
    for name, mem in inspect.getmembers(target):
        if inspect.ismodule(mem):
            my_module_name: str = mem.__name__
            my_module_name = my_module_name.split('.')[-1]
            header = header + "\nfrom {} import {}\n".format(target.__name__, my_module_name)
            generate_doc_for_module(mem)
        elif inspect.isclass(mem):
            class_doc = generate_class_doc(mem)
            doc = doc + class_doc
    try:
        doc = header + doc
        f = open(p + "/__init__.py", "w", encoding='utf-8')
        f.write(doc)
        f.close()
    except OSError as err:
        assert False
    return


class MainPanel(WindowBase):
    main_scene: 'luna.Scene' = None
    window_name = "Scene Window"

    def __init__(self) -> None:
        super().__init__()

        self.main_scene = None

        from ui.hierarchy_panel import HierarchyPanel
        from ui.inspector_panel import InspectorPanel
        from ui.scene_panel import ScenePanel
        from ui.library_panel import LibraryPanel
        from ui.asset_inspector_panel import AssetPanel
        #from ui.imgui_demo import DemoPanel

        self.hierarchy_panel = self.add_panel(HierarchyPanel)
        #self.demo_panel = self.add_panel(DemoPanel)
        self.scene_panel = self.add_panel(ScenePanel)
        self.library_panel = self.add_panel(LibraryPanel)
        self.inspector = self.add_panel(InspectorPanel)
        self.asset_panel = self.add_panel(AssetPanel)

    def on_title(self):
        proj_dir = EditorModule.instance().project_dir
        scene_path = EditorModule.instance().default_scene_path
        app_title = imgui.ICON_FA_MOON + "  Luna Editor"
        self.title = "{}\t\t{}\t\t{}".format(app_title, proj_dir, scene_path)

    def set_main_scene(self, scn):
        old_scn = None
        if self.main_scene != scn:
            old_scn = self.main_scene

        self.main_scene = scn
        self.hierarchy_panel.set_scene(scn)
        self.scene_panel.set_scene(scn)
        if scn:
            self.show_status("打开场景: {}".format(scn.path))
            game_module.add_scene(self.main_scene)
        if old_scn:
            old_scn.destroy()

    def on_file_menu(self):
        if imgui.begin_menu("文件", True):
            if imgui.menu_item("打开项目"):
                name = tkinter.filedialog.askdirectory(initialdir=platform_module.engine_dir)
                if name:
                    platform_module.set_project_dir(name)
            if imgui.menu_item("新建场景"):
                name = tkinter.filedialog.asksaveasfilename(filetypes=(("scene files", "*.scn"),),
                                                            initialfile="Scene.scn",
                                                          initialdir=platform_module.project_dir + "/assets")
                if name and name.startswith(platform_module.project_dir):
                    engine_path = os.path.relpath(name, platform_module.project_dir)
                    new_scn = asset_module.new_asset(engine_path, luna.Scene)
                    self.set_main_scene(None)
                    self.set_main_scene(new_scn)

            if imgui.menu_item("打开场景"):
                name = tkinter.filedialog.askopenfilename(filetypes=(("scene files", "*.scn"),),
                                                          initialdir=platform_module.project_dir + "/assets")
                if name:
                    engine_path = os.path.relpath(name, platform_module.project_dir)
                    scn = asset_module.load_asset(engine_path, luna.Scene)
                    self.set_main_scene(scn)
            if imgui.menu_item("导入资源"):
                name = tkinter.filedialog.askopenfilename(filetypes=(("obj files", "*.obj"),("fbx files", "*.fbx"),("gltf files", "*.gltf"),),
                                                          initialdir=platform_module.project_dir + "/assets")
                file_with_extension = os.path.basename(name)
                (file_without_extension,file_extension) = os.path.splitext(file_with_extension)
                luna.resource_parse.import_resource_file(name,platform_module.project_dir + "/assets",file_without_extension,file_extension)
                
            if imgui.menu_item("生成 Python API"):
                self.show_status("生成PythonAPI中")
                generate_doc_for_module(luna)
            if imgui.menu_item("保存场景"):
                asset_module.save_asset(self.main_scene, self.main_scene.path)
            if imgui.menu_item("退出"):
                EditorModule.instance().open_asset(None)

            imgui.end_menu()

    def on_help_menu(self):
        if imgui.begin_menu("帮助", True):
            if imgui.menu_item("关于"):
                self.show_message_box("Luna Editor 0.1", "made by Isak Wong, Pancy Star")
            imgui.end_menu()

    def on_tool_menu(self):
        if imgui.begin_menu("工具", True):
            if imgui.menu_item("关于"):
                self.show_message_box("Luna Editor 0.1", "made by Isak Wong, Pancy Star")
            imgui.end_menu()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        if imgui.begin_menu_bar():
            self.on_file_menu()
            self.on_tool_menu()
            self.on_help_menu()
            imgui.end_menu_bar()

