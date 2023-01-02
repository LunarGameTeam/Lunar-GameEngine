import typing
from typing import Type, List, Any

import luna
from luna import LObject, imgui

editors = dict()


def register_type_editor(cls):
    global editors
    editors[cls.target_type] = cls
    return cls


def get_type_editor(cls):
    global editors
    return editors.get(cls, None)


def create_editor(target):
    editor_cls = get_type_editor(target.__class__)
    return editor_cls(target)


@register_type_editor
class EditorBase(object):
    child_editor_list: list['EditorBase']
    target: luna.LObject
    target_type = luna.LObject

    def __init__(self, target):
        super(EditorBase, self).__init__()
        self.target = target
        self.child_editor_list = self.create_child_editor()
        self.indent = 0

    def imgui_obj_property(self, comp, prop):
        if prop.name == "name":
            return
        val = getattr(comp, prop.name)
        # if issubclass(prop.type, list):
        #     for i in range(0, len(val)):
        #         element = val[i]
        #         pass
        # elif issubclass(prop.type, luna.LBasicAsset):
        #     luna.imgui.align_text_to_frame_padding()
        #     luna.imgui.text(prop.name)
        #     imgui.same_line(16 + self.width, 16)
        #     changed, new_val = luna.imgui.input("##" + prop.name, val.path, 0)
        #     if changed:
        #         pass
        # elif issubclass(prop.type, luna.LObject):
        #     luna.imgui.align_text_to_frame_padding()
        #     luna.imgui.text(prop.name)
        #     self.imgui_property(val)
        if prop.type == luna.LQuaternion:
            luna.imgui.align_text_to_frame_padding()
            luna.imgui.text(prop.name)
            imgui.same_line(16 + self.indent, 16)
            euler = luna.math.to_euler(val)
            changed, new_val = luna.imgui.drag_float3("##" + prop.name, euler, 0.2, 1, -1)
            if changed:
                q = luna.math.from_euler(new_val)
                setattr(comp, prop.name, q)
        elif prop.type == bool:
            luna.imgui.align_text_to_frame_padding()
            luna.imgui.text(prop.name)
            imgui.same_line(16 + self.indent, 16)
            changed, new_val = luna.imgui.checkbox("##" + prop.name, val)
            if changed:
                setattr(comp, prop.name, new_val)
        elif prop.type == luna.LVector3f:
            luna.imgui.align_text_to_frame_padding()
            luna.imgui.text(prop.name)
            imgui.same_line(16 + self.indent, 16)
            changed, new_val = luna.imgui.drag_float3("##" + prop.name, val, 0.2, 1, -1)
            if changed:
                setattr(comp, prop.name, new_val)

    def on_imgui(self):
        target_type: typing.Type[luna.LObject] = self.target.__class__
        properties: list[luna.LProperty] = target_type.get_properties()
        for prop in properties:
            self.imgui_obj_property(self.target, prop)
        for editor in self.child_editor_list:
            editor.on_imgui()

    def create_child_editor(self):
        return []


@register_type_editor
class EntityEditor(EditorBase):
    target_type = luna.Entity
    target: luna.Entity

    def __init__(self, target):
        super(EntityEditor, self).__init__(target)

    def on_imgui(self):
        for it in self.child_editor_list:
            it.on_imgui()

    def create_child_editor(self):
        count = self.target.get_component_count()
        child_editor = []
        for i in range(0, count):
            comp = self.target.get_component_at(i)
            child_editor.append(ComponentEditor(comp))
        return child_editor


@register_type_editor
class ComponentEditor(EditorBase):
    target_type = luna.Component

    def __init__(self, target):
        super(ComponentEditor, self).__init__(target)

    def on_imgui(self):
        target_type = self.target.__class__
        properties = target_type.get_properties()
        is_open = imgui.tree_node(id(self.target), imgui.ImGuiTreeNodeFlags_DefaultOpen, target_type.__name__)
        if is_open:
            self.indent = 0
            for prop in properties:
                self.indent = max(self.indent, luna.imgui.calc_text_size(prop.name).x)

            for prop in properties:
                self.imgui_obj_property(self.target, prop)
            imgui.tree_pop()


@register_type_editor
class MeshRendererEditor(ComponentEditor):
    target_type = luna.MeshRenderer

    def __init__(self, target):
        super(MeshRendererEditor, self).__init__(target)

