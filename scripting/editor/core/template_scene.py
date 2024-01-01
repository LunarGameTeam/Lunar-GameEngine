import luna
from editor.core.editor_module import game_module


class TemplateSceneView(object):

    def __init__(self, scene):
        self.scene = scene
        self.scene_texture = None

        self.camera = None  #type: luna.CameraComponent
        self.find_camera()

        self.need_update_texture = True

        game_module.add_scene(scene)

    def find_entity(self, name: str) -> luna.Entity:

        count = self.scene.get_entity_count()
        for i in range(0, count):
            entity = self.scene.get_entity_at(i)
            if entity.name == name:
                return entity
        return None

    def find_camera(self):

        count = self.scene.get_entity_count()
        for i in range(0, count):
            entity = self.scene.get_entity_at(i)
            camera = entity.get_component(luna.CameraComponent)
            if camera:
                entity.serializable = False
                self.camera = camera
                continue

        if not self.camera:
            entity = self.scene.create_entity("EditorCamera")
            entity.serializable = False
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
        #self.scene_texture = rt.color_texture
