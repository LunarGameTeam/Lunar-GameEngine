import importlib
import os
import sys


work_dir = os.getcwd()
bin_dir = os.path.dirname(sys.executable)
editor_dir = work_dir + "\\editor"
packages_dir = work_dir + "\\third-party\\site-packages"


def init_editor():
    # app = QtWidgets.QApplication([])
    # widget = QWidget()
    # widget.resize(800, 600)
    # widget.show()
    # global is_looping
    # def f():
    # 	global is_looping
    # 	is_looping = False
    # QCoreApplication.instance().aboutToQuit.connect(f)
    # while is_looping:
    # 	QCoreApplication.instance().processEvents()
    # return
    import luna

    sys.path.append(editor_dir)
    sys.path.append(packages_dir)

    from core.hot_patch import watch_scripts
    watch_scripts(editor_dir)

    luna.load_library("imgui_binding.dll")
    luna.load_library("game.dll")

    app = luna.LApplication.instance()

    from core.test import binding_test
    # 先做 binding test 再执行
    binding_test()

    from core.editor_module import EditorModule
    luna.add_module(EditorModule.instance())

    app.main_loop()


if __name__ == '__main__':
    sys.path.append(bin_dir)
    import luna

    luna.add_library_dir(bin_dir)
    init_editor()
