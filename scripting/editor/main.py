import os
import sys

work_dir = os.getcwd()
bin_dir = os.path.dirname(sys.executable)
editor_dir = work_dir + "\\scripting"
packages_dir = work_dir + "\\third-party\\site-packages"


def init_editor():
    import luna

    sys.path.append(editor_dir)
    sys.path.append(packages_dir)

    from editor.core.hot_patch import watch_scripts
    watch_scripts(editor_dir)

    luna.load_library("ImGuiScript.dll")
    luna.load_library("Game.dll")
    luna.load_library("AssetImport.dll")
    app = luna.LunaCore.instance()

    from unit_test.test import binding_test
    # 先做 binding test 再执行
    binding_test()

    from editor.core.editor_module import EditorModule
    luna.add_module(EditorModule.instance())

    platform_module = luna.get_module(luna.PlatformModule)
    import tkinter.filedialog
    while not platform_module.project_dir:
        name = tkinter.filedialog.askdirectory(initialdir=platform_module.engine_dir)
        if name:
            EditorModule.instance().project_dir = name
            platform_module.set_project_dir(name)
    app.main_loop()


if __name__ == '__main__':
    sys.path.append(bin_dir)
    import luna

    luna.add_library_dir(bin_dir)
    init_editor()
