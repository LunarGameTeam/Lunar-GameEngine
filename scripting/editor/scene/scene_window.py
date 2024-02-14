import inspect
import os
import tkinter
import types

import luna
from editor.core.editor_module import EditorModule, game_module, asset_module, platform_module
from editor.ui.panel import WindowBase
from luna import imgui


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


class SceneWindow(WindowBase):
    main_scene: 'luna.Scene' = None
    window_name = "Scene Window"

    def __init__(self) -> None:
        super().__init__()

        self.main_scene = None


        from editor.ui.inspector_panel import InspectorPanel

        from editor.ui.library_panel import LibraryPanel
        from editor.ui.imgui_demo import DemoPanel
        from editor.scene.scene_hierarchy_panel import HierarchyPanel
        self.hierarchy_panel = self.create_panel(HierarchyPanel)
        self.demo_panel = self.create_panel(DemoPanel)
        from editor.scene.scene_view_panel import SceneAssetViewPanel
        self.scene_panel = self.create_panel(SceneAssetViewPanel)

        self.library_panel = self.add_panel(LibraryPanel.get_singleton())
        self.inspector = self.create_panel(InspectorPanel)

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
        if old_scn:
            old_scn.destroy()

    def on_activate(self):
        if self.main_scene:
            self.main_scene.renderable = True

    def on_deactivate(self):
        if self.main_scene:
            self.main_scene.renderable = False

    def on_toolbar_menu(self):
        super().on_toolbar_menu()
        if imgui.begin_menu("文件", True):
            if imgui.menu_item("新建场景"):
                name = tkinter.filedialog.asksaveasfilename(filetypes=(("scene files", "*.scn"),),
                                                            initialfile="Scene.scn",
                                                            initialdir=platform_module.project_dir + "/assets")
                if name and name.startswith(platform_module.project_dir):
                    engine_path = os.path.relpath(name, platform_module.project_dir)
                    new_scn = asset_module.new_asset(engine_path, luna.Scene)
                    self.set_main_scene(None)
                    self.set_main_scene(new_scn)

            if imgui.menu_item("导入到当前场景"):
                scene_nodes_asset = luna.LSceneAssetNodeMessage()
                name = tkinter.filedialog.askopenfilename(
                    filetypes=(("obj files", "*.obj"), ("fbx files", "*.fbx"), ("gltf files", "*.gltf"),),
                    initialdir=platform_module.project_dir + "/assets")
                file_with_extension = os.path.basename(name)
                (file_without_extension, file_extension) = os.path.splitext(file_with_extension)
                luna.editor.import_scene_res(name, "/assets/demoSceneAsset", file_without_extension,
                                        file_extension,scene_nodes_asset)
                import_node_count = scene_nodes_asset.get_asset_node_count()
                for idx in range(0, import_node_count):
                    node_name = scene_nodes_asset.get_node_name(idx)
                    node_mesh_name = scene_nodes_asset.get_node_mesh_asset_name(idx)
                    node_mat_name = scene_nodes_asset.get_node_mat_asset_name(idx)
                    node_translation = scene_nodes_asset.get_node_translation(idx)
                    node_rotation = scene_nodes_asset.get_node_rotation(idx)
                    node_scale = scene_nodes_asset.get_node_scale(idx)
                    new_entity = self.main_scene.create_entity(node_name)
                    transform = new_entity.get_component(luna.Transform)
                    transform.local_position = node_translation
                    transform.local_rotation = node_rotation
                    transform.local_scale = node_scale
                    renderer = new_entity.add_component_without_create(luna.StaticMeshRenderer)
                    renderer.mesh = asset_module.load_asset(node_mesh_name, luna.MeshAsset)
                    renderer.material = asset_module.load_asset(node_mat_name, luna.MaterialTemplateAsset)
                    renderer.force_create()



            if imgui.menu_item("打开场景"):
                name = tkinter.filedialog.askopenfilename(filetypes=(("scene files", "*.scn"),),
                                                          initialdir=platform_module.project_dir + "/assets")
                if name:
                    engine_path = os.path.relpath(name, platform_module.project_dir)
                    scn = asset_module.load_asset(engine_path, luna.Scene)
                    self.set_main_scene(scn)
            if imgui.menu_item("保存场景"):
                asset_module.save_asset(self.main_scene, self.main_scene.path)

            if imgui.menu_item("生成 Python API"):
                self.show_status("生成PythonAPI中")
                generate_doc_for_module(luna)
            imgui.end_menu()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
