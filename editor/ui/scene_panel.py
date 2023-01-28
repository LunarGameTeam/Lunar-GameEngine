import math

import luna
from luna import imgui
from ui.panel import PanelBase


class ScenePanel(PanelBase):
    main_light: luna.DirectionLightComponent
    camera: luna.CameraComponent
    scene: luna.Scene

    def __init__(self) -> None:
        super().__init__()
        self.title = "Scene"
        self.scene = None
        self.has_menubar = True
        self.scene_texture = None
        self.camera = None
        self.main_light = None
        self.last_min = None
        self.last_max = None
        self.dragging = False
        self.need_update_texture = False
        self.operation = imgui.gizmos.Operation_TRANSLATE

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

    def create_geometry(self, mesh_asset):
        if self.scene:
            entity = self.scene.create_entity("Cube")
            renderer = entity.add_component(luna.MeshRenderer)
            from core.editor_module import asset_module
            renderer.mesh = asset_module.load_asset(mesh_asset, luna.MeshAsset)
            renderer.material = asset_module.load_asset("/assets/built-in/Pbr.mat", luna.MaterialTemplateAsset)

    def create_point_light(self):
        if self.scene:
            entity = self.scene.create_entity("PointLight")
            transform = entity.add_component(luna.Transform)
            light = entity.add_component(luna.PointLightComponent)
            light.color = luna.LVector4f(1, 1, 1, 1)
            light.intensity = 1.0

    def create_direction_light(self):
        if self.scene:
            entity = self.scene.create_entity("Main Direction Light")
            transform = entity.add_component(luna.Transform)
            light = entity.add_component(luna.DirectionLightComponent)
            light.color = luna.LVector4f(1, 1, 1, 1)
            light.intensity = 1.0

    def on_menu(self):
        if luna.imgui.begin_menu_bar():
            if luna.imgui.begin_menu("创建", True):
                if imgui.menu_item("Cube"):
                    self.create_geometry("/assets/built-in/Geometry/Box.obj")
                if imgui.menu_item("Sphere"):
                    self.create_geometry("/assets/built-in/Geometry/Sphere.obj")
                if imgui.menu_item("Plane"):
                    self.create_geometry("/assets/built-in/Geometry/Plane.obj")
                if imgui.menu_item("PointLight"):
                    self.create_point_light()
                if imgui.menu_item("Directional Light"):
                    if self.scene:
                        entity = self.scene.create_entity("PointLight")
                        transform = entity.add_component(luna.Transform)
                        light = entity.add_component(luna.DirectionLightComponent)
                        light.color = luna.LVector4f(1, 1, 1, 1)
                        light.intensity = 1.0
                    pass
                luna.imgui.end_menu()
            luna.imgui.end_menu_bar()

    def on_gizmos(self):
        key_pressed = False
        if imgui.is_key_pressed(imgui.ImGuiKey_W, False):
            key_pressed = True
            self.operation = imgui.gizmos.Operation_TRANSLATE
        if imgui.is_key_pressed(imgui.ImGuiKey_R, False):
            key_pressed = True
            self.operation = imgui.gizmos.Operation_ROTATE

        from ui.hierarchy_panel import HierarchyPanel
        selected: 'luna.Entity' = self.parent_window.get_panel(HierarchyPanel).selected_entity

        imgui.gizmos.set_rect(self.window_pos.x + self.scene_pos.x, self.window_pos.y + self.scene_pos.y,
                              self.scene_content.x,
                              self.scene_content.y)

        view_mat = self.camera.view_matrix
        proj_mat = self.camera.proj_matrix
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

        content = luna.imgui.get_content_region_avail()

        vmin = imgui.get_window_content_min()
        vmax = imgui.get_window_content_max()

        self.scene_pos = luna.imgui.get_cursor_pos()
        self.scene_content = content
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
