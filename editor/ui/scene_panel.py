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
        self.last_min = None
        self.last_max = None
        self.dragging = False
        self.need_update_texture = False
    def create_editor_camera(self, scene: 'luna.Scene'):
        e: 'luna.Entity' = scene.create_entity("_EditorCamera", None)
        camera = e.add_component(luna.CameraComponent)

    def set_scene(self, scene):
        self.scene = scene
        count = self.scene.get_entity_count()
        for i in range(0, count):
            entity = self.scene.get_entity_at(i)
            camera = entity.get_component(luna.CameraComponent)
            if camera:
                self.camera = camera
        self.scene_texture = self.camera.render_target.color_texture
        self.need_update_texture = True

    def resize_scene_texture(self, width, height):
        rt = self.camera.render_target
        rt.width = int(width)
        rt.height = int(height)
        rt.update()
        if self.camera:
            self.camera.aspect = rt.width / rt.height
        self.scene_texture = rt.color_texture
    def handle_move(self):
        delta = luna.LVector3f(0, 0, 0)
        if imgui.is_key_down(imgui.ImGuiKey_W):
            delta.z += 1
        if imgui.is_key_down(imgui.ImGuiKey_A):
            delta.x -= 1
        if imgui.is_key_down(imgui.ImGuiKey_S):
            delta.z -= 1
        if imgui.is_key_down(imgui.ImGuiKey_D):
            delta.x += 1
        if imgui.is_key_down(imgui.ImGuiKey_Q):
            delta.y += 1
        if imgui.is_key_down(imgui.ImGuiKey_E):
            delta.y -= 1
        if delta.size() > 0.01:
            delta.normalize()
            direction = self.camera.transform.local_rotation * delta
            direction.normalize()
            self.camera.direction = direction
            self.camera.fly_speed = 10
        else:
            self.camera.fly_speed = 0

    def handle_rotate(self):
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
    def on_menu(self):
        if luna.imgui.begin_menu_bar():
            if luna.imgui.begin_menu("创建", True):
                if imgui.menu_item("Cube"):
                    pass
                if imgui.menu_item("Sphere"):
                    pass
                luna.imgui.end_menu()
            luna.imgui.end_menu_bar()

    def on_imgui(self) -> None:
        super(PyScenePanel, self).on_imgui()
        content = luna.imgui.get_content_region_avail()
        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()

        if self.scene_texture:
            luna.imgui.image(self.scene_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))

        if imgui.is_mouse_dragging(0, -1.0):
            if vmin != self.last_min or vmax != self.last_max:
                self.dragging = True
            self.last_min = imgui.get_window_content_min()
            self.last_max = imgui.get_window_content_max()

        if self.need_update_texture:
            if content.x > 0 and content.y > 0:
                self.resize_scene_texture(content.x, content.y)
                self.need_update_texture = False

        if self.dragging:
            if imgui.is_mouse_released(0):
                self.resize_scene_texture(vmax.x - vmin.x, vmax.y - vmin.y)
                self.dragging = False

        if self.camera:
            self.handle_move()
            self.handle_rotate()

        self.on_menu()

