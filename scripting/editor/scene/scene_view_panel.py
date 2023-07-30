import math

import luna
from editor.scene.scene_hierarchy_panel import HierarchyPanel
from luna import imgui
from editor.ui.panel import PanelBase


class ScenePanel(PanelBase):
    main_light: luna.DirectionLightComponent
    camera: luna.CameraComponent
    scene: luna.Scene

    def __init__(self) -> None:
        super().__init__()
        self.title = "Scene"
        self.scene = None

        self.main_light = None
        self.point_light = None
        self.last_min = None
        self.last_max = None
        self.dragging = False
        self.need_update_texture = False
        self.rad = 0
        self.has_menu = False
        self.operation = imgui.gizmos.Operation_TRANSLATE
        self.loaded_scenes = {}
        self.current_scene_view = None

    def create_editor_camera(self, scene: 'luna.Scene'):
        e: 'luna.Entity' = scene.create_entity("_EditorCamera", None)
        camera = e.add_component(luna.CameraComponent)

    def set_scene(self, scene):

        self.scene = scene

        if scene:
            if scene not in self.loaded_scenes:
                from editor.core.template_scene import TemplateSceneView
                self.loaded_scenes[scene] = TemplateSceneView(scene)
                self.current_scene_view = self.loaded_scenes[scene]
                self.current_scene_view.find_camera()

            self.current_scene_view = self.loaded_scenes[scene]

        if not self.scene:
            self.main_light = None
            return

        count = self.scene.get_entity_count()

        for i in range(0, count):
            entity = self.scene.get_entity_at(i)

            main_light = entity.get_component(luna.DirectionLightComponent)
            if main_light:
                self.main_light = main_light

            point_light = entity.get_component(luna.PointLightComponent)
            if point_light and self.point_light is None:
                self.point_light = point_light

        self.need_update_texture = True

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
        if imgui.is_key_down(imgui.ImGuiKey_E):
            delta.y += 1
        if imgui.is_key_down(imgui.ImGuiKey_Q):
            delta.y -= 1
        if delta.size() > 0.01:
            delta.normalize()
            direction = self.current_scene_view.camera.transform.local_rotation * delta
            direction.normalize()
            self.current_scene_view.camera.direction = direction
            self.current_scene_view.camera.fly_speed = 10
        else:
            self.current_scene_view.camera.fly_speed = 0

    def handle_rotate(self):
        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()
        vmin.x += imgui.get_window_pos().x
        vmin.y += imgui.get_window_pos().y
        vmax.x += imgui.get_window_pos().x
        vmax.y += imgui.get_window_pos().y
        if imgui.is_mouse_hovering_rect(vmin, vmax, True) and self.current_scene_view.camera and imgui.is_mouse_dragging(1, -1):
            delta = imgui.get_mouse_drag_delta(1, -1.0)
            transform = self.current_scene_view.camera.transform
            delta_y = delta.y / 500.0
            delta_x = delta.x / 500.0
            v = luna.math.angle_axis(delta_y * math.pi, luna.LVector3f(1, 0, 0))
            h = luna.math.angle_axis(delta_x * math.pi, luna.LVector3f(0, 1, 0))
            rotation = transform.local_rotation
            rotation = h * rotation * v
            transform.local_rotation = rotation
            imgui.reset_mouse_drag_delta(1)


    def on_menu(self):
        pass

    def on_gizmos(self):
        key_pressed = False
        if imgui.is_key_pressed(imgui.ImGuiKey_W, False):
            key_pressed = True
            self.operation = imgui.gizmos.Operation_TRANSLATE
        if imgui.is_key_pressed(imgui.ImGuiKey_R, False):
            key_pressed = True
            self.operation = imgui.gizmos.Operation_ROTATE

        selected: 'luna.Entity' = self.parent_window.get_panel(HierarchyPanel).selected_entity

        imgui.gizmos.set_rect(self.window_pos.x + self.scene_pos.x, self.window_pos.y + self.scene_pos.y,
                              self.scene_content.x,
                              self.scene_content.y)

        view_mat = self.current_scene_view.camera.view_matrix
        proj_mat = self.current_scene_view.camera.proj_matrix
        if selected:
            transform = selected.get_component(luna.Transform)
            mat = transform.world_matrix
            changed, new_matrix = imgui.gizmos.manipulate(view_mat, proj_mat, self.operation,
                                                          imgui.gizmos.Mode_WORLD, mat)
            if changed:
                new_matrix: 'luna.LMatrix4f'
                new_pos = new_matrix.translation()
                new_rotation = new_matrix.rotation()
                transform.local_position = new_pos
                transform.local_rotation = new_rotation
        return key_pressed or imgui.gizmos.is_using()

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        imgui.gizmos.set_orthographic(False)
        imgui.gizmos.begin_frame()
        imgui.gizmos.set_draw_list()

        if self.point_light:
            x = 4 + math.cos(self.rad) * 6
            y = math.sin(self.rad) * 6
            z = -3
            # self.point_light.entity.get_component(luna.Transform).local_position = luna.LVector3f(x, y ,z)
            self.rad += math.pi * 0.2 * delta_time

        content = luna.imgui.get_content_region_avail()

        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()

        self.scene_pos = luna.imgui.get_cursor_pos()
        self.scene_content = content
        if self.current_scene_view:
            luna.imgui.image(self.current_scene_view.scene_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))

        if imgui.is_mouse_dragging(0, -1.0):
            if vmin != self.last_min or vmax != self.last_max:
                self.dragging = True
            self.last_min = imgui.get_window_content_min()
            self.last_max = imgui.get_window_content_max()

        if self.need_update_texture:
            if content.x > 0 and content.y > 0:
                self.current_scene_view.resize_scene_texture(content.x, content.y)
                self.need_update_texture = False

        if self.dragging and self.current_scene_view:
            if imgui.is_mouse_released(0):
                self.current_scene_view.resize_scene_texture(vmax.x - vmin.x, vmax.y - vmin.y)
                self.dragging = False

        self.on_menu()

        if not self.current_scene_view.camera:
            return

        if self.on_gizmos():
            pass
        elif self.handle_move():
            pass
        elif self.handle_rotate():
            pass
