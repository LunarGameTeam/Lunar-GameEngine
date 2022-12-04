import os
import sys



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

    from ui.hierarchy_editor import PyHierarchyEditor
    from ui.main_editor import PyMainEditor
    from ui.inspector import PyInspectorEditor
    from ui.scene_editor import PySceneEditor
    from ui.library_editor import PyLibraryEditor

    asset_module = luna.get_module(luna.AssetModule)
    render_module = luna.get_module(luna.RenderModule)
    scene_module = luna.get_module(luna.SceneModule)
    app.run()

    scn = asset_module.load_asset("/assets/test.scn", luna.Scene)
    entity = scn.find_entity("MainCamera")
    camera = entity.get_component(luna.CameraComponent)
    props = luna.CameraComponent.get_properties()

    scene_module.add_scene(scn)

    editor_module = luna.get_module(luna.editor.EditorModule)
    editor_module.register_editor(PyMainEditor())
    editor_module.register_editor(PySceneEditor())
    editor_module.register_editor(PyLibraryEditor())
    editor_module.register_editor(PyHierarchyEditor())
    editor_module.register_editor(PyInspectorEditor())
    
    app.main_loop()


if __name__ == '__main__':
    bin_dir = os.getcwd() + "\\bin"
    sys.path.append(bin_dir)
    is_looping = True
    init_editor()

