import os
import sys

def test():
    import luna
    v = luna.LVector3f(1, 1, 1)

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
    luna.load_library("editor.dll")

    app = luna.LApplication.instance()

    from test import test_luna
    test_luna()

    app.run()
    test()
    # from core.editor_core import EditorCore
    # EditorCore.instance()
    app.main_loop()


if __name__ == '__main__':
    bin_dir = os.getcwd() + "\\bin"
    sys.path.append(bin_dir)
    is_looping = True
    init_editor()

