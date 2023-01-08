import math
import os
import luna
from luna import editor
from luna import imgui


class PyScenePanel(editor.ScenePanel):
    camera: luna.CameraComponent
    scene: luna.Scene

    def __init__(self) -> None:
        super(PyScenePanel, self).__init__()
        self.scene = None
        self.has_menubar = True
        self.scene_texture = None
        self.camera = None

    def set_scene(self, scene):
        self.scene = scene
        count = self.scene.get_entity_count()
        for i in range(0, count):
            entity = self.scene.get_entity_at(i)
            camera = entity.get_component(luna.CameraComponent)
            if camera:
                self.camera = camera
        from core.editor_core import render_module
        self.scene_texture = render_module.get_imgui_texture("SceneColor")

    def on_imgui(self) -> None:
        super(PyScenePanel, self).on_imgui()
        content = luna.imgui.get_content_region_avail()
        luna.imgui.image(self.scene_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))
        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()
        vmin.x += imgui.get_window_pos().x
        vmin.y += imgui.get_window_pos().y
        vmax.x += imgui.get_window_pos().x
        vmax.y += imgui.get_window_pos().y
        if imgui.is_mouse_hovering_rect(vmin, vmax, True) and self.camera:
            delta = imgui.get_mouse_drag_delta(0, -1.0)
            transform = self.camera.transform
            delta_y = delta.y / 500.0
            delta_x = delta.x / 500.0
            v = luna.math.angle_axis(delta_y * math.pi, luna.LVector3f(1, 0, 0))
            h = luna.math.angle_axis(delta_x * math.pi, luna.LVector3f(0, 1, 0))
            rotation = transform.local_rotation
            rotation = h * rotation * v
            transform.local_rotation = rotation
            imgui.reset_mouse_drag_delta(0)

        if luna.imgui.begin_menu_bar():
            if luna.imgui.begin_menu("创建", True):
                if imgui.menu_item("Cube"):
                    pass
                if imgui.menu_item("Sphere"):
                    pass
                luna.imgui.end_menu()
            luna.imgui.end_menu_bar()
