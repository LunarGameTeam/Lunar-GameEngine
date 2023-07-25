import math

import luna
from luna import imgui
from editor.ui.panel import PanelBase
asset_module: 'luna.AssetModule' = luna.get_module(luna.AssetModule)

class CustomSceneViewPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.title = "Scene"
        self.scene = None
        self.scene_texture = None
        self.camera = None
        self.point_light = None
        self.main_light = None
        self.dragging = False
        self.need_update_texture = False
        global asset_module
        #scn = asset_module.load_asset("/assets/built-in/TemplateScene/EmptyView.scn", luna.Scene)
        #self.set_scene(scn)

    def create_editor_camera(self, scene: 'luna.Scene'):
        e: 'luna.Entity' = scene.create_entity("_EditorCamera", None)
        camera = e.add_component(luna.CameraComponent)

    def set_scene(self, scene):
        self.scene = scene
        if not self.scene:
            self.camera = None
            self.main_light = None
            self.scene_texture = None
            return

        count = self.scene.get_entity_count()

        for i in range(0, count):
            entity = self.scene.get_entity_at(i)
            camera = entity.get_component(luna.CameraComponent)
            if camera:
                self.camera = camera
                continue
            main_light = entity.get_component(luna.DirectionLightComponent)
            if main_light:
                self.main_light = main_light

            point_light = entity.get_component(luna.PointLightComponent)
            if point_light and self.point_light is None:
                self.point_light = point_light

        # if not self.main_light:
        #     entity = self.scene.create_entity("MainLight")
        #     self.main_light = entity.add_component(luna.DirectionLightComponent)

        if not self.camera:
            entity = self.scene.create_entity("EditorCamera")
            self.camera = entity.add_component(luna.CameraComponent)

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
        if imgui.is_key_down(imgui.ImGuiKey_E):
            delta.y += 1
        if imgui.is_key_down(imgui.ImGuiKey_Q):
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
        if imgui.is_mouse_hovering_rect(vmin, vmax, True) and self.camera and imgui.is_mouse_dragging(1, -1):
            delta = imgui.get_mouse_drag_delta(1, -1.0)
            transform = self.camera.transform
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
        pass

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)


        content = luna.imgui.get_content_region_avail()

        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()

        self.scene_pos = luna.imgui.get_cursor_pos()
        self.scene_content = content
        if self.scene_texture:
            luna.imgui.image(self.scene_texture, content, luna.LVector2f(0, 0), luna.LVector2f(1, 1))

        if imgui.is_mouse_dragging(0, -1.0):
            pass
            #if vmin != self.last_min or vmax != self.last_max:
            #    self.dragging = True
            #self.last_min = imgui.get_window_content_min()
           # self.last_max = imgui.get_window_content_max()

        if self.need_update_texture:
            if content.x > 0 and content.y > 0:
                self.resize_scene_texture(content.x, content.y)
                self.need_update_texture = False

        if self.dragging and self.scene_texture:
            if imgui.is_mouse_released(0):
                self.resize_scene_texture(vmax.x - vmin.x, vmax.y - vmin.y)
                self.dragging = False

        self.on_menu()

        if not self.camera:
            return

        if self.on_gizmos():
            pass
        elif self.handle_move():
            pass
        elif self.handle_rotate():
            pass
