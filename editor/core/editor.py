import typing

import luna
from core.editor_module import asset_module
from luna import imgui

editors = dict()


def register_type_editor(cls):
    global editors
    editors[cls.target_type] = cls
    return cls


def get_type_editor(cls):
    global editors
    base = cls
    while base:
        if base in editors:
            return editors.get(base)
        base = base.__base__
    return None


def create_editor(target):
    editor_cls = get_type_editor(target.__class__)
    return editor_cls(target)


@register_type_editor
class EditorBase(object):
    child_editor_list: list['EditorBase']
    target: luna.LObject
    target_type = luna.LObject

    def __init__(self, target):
        super().__init__()
        self.target = target
        self.child_editor_list = self.create_child_editor()
        self.indent = self.get_indent()

    def get_indent(self):
        indent = 0
        target_type = self.target.__class__
        properties = target_type.get_properties()
        for prop in properties:
            indent = max(indent, imgui.calc_text_size(prop.name).x)
        return indent

    def imgui_obj_property(self, comp, prop_name, prop_type):
        if prop_name == "name":
            return
        val = getattr(comp, prop_name)
        if issubclass(prop_type, luna.Asset):
            imgui.align_text_to_frame_padding()
            imgui.text(prop_name)
            imgui.same_line(16 + self.indent, 16)
            changed, new_val = imgui.input("##" + prop_name, val.path, 0)
            if imgui.begin_drag_drop_target():
                accept_drop = imgui.accept_drag_drop_payload("LibraryItem", 0)
                if accept_drop:
                    f = accept_drop["data"]
                    new_val = asset_module.load_asset(f.path, prop_type)
                    setattr(comp, prop_name, new_val)
                imgui.end_drag_drop_target()
            return changed, new_val
        elif prop_type == luna.LQuaternion:
            imgui.align_text_to_frame_padding()
            imgui.text(prop_name)
            imgui.same_line(16 + self.indent, 16)
            euler = luna.math.to_euler(val)
            changed, new_val = imgui.drag_float3("##" + prop_name, euler, 0.2, 1, -1)
            if changed:
                q = luna.math.from_euler(new_val)
                setattr(comp, prop_name, q)
            return changed, new_val
        elif prop_type == bool:
            imgui.align_text_to_frame_padding()
            imgui.text(prop_name)
            imgui.same_line(16 + self.indent, 16)
            changed, new_val = imgui.checkbox("##" + prop_name, val)
            if changed:
                setattr(comp, prop_name, new_val)
            return changed, new_val
        elif prop_type == luna.LVector3f:
            imgui.align_text_to_frame_padding()
            imgui.text(prop_name)
            imgui.same_line(16 + self.indent, 16)
            changed, new_val = imgui.drag_float3("##" + prop_name, val, 0.2, 1, -1)
            if changed:
                setattr(comp, prop_name, new_val)
            return changed, new_val
        elif prop_type == float:
            imgui.align_text_to_frame_padding()
            imgui.text(prop_name)
            imgui.same_line(16 + self.indent, 16)
            changed, new_val = imgui.drag_float("##" + prop_name, val, 0.2, 1, -1)
            if changed:
                setattr(comp, prop_name, new_val)
            return changed, new_val

    def on_imgui(self):
        target_type: typing.Type[luna.LObject] = self.target.__class__
        properties: list[luna.LProperty] = target_type.get_properties()
        for prop in properties:
            self.imgui_obj_property(self.target, prop.name, prop.type)
        for editor in self.child_editor_list:
            editor.on_imgui()

    def create_child_editor(self):
        return []


@register_type_editor
class EntityEditor(EditorBase):
    target_type = luna.Entity
    target: luna.Entity

    def __init__(self, target):
        super().__init__(target)

    def on_imgui(self):
        for it in self.child_editor_list:
            it.on_imgui()

    def create_child_editor(self):
        child_editor: list[EditorBase] = []
        indent = 0
        for i in range(0, self.target.component_count):
            comp = self.target.get_component_at(i)
            editor_type = get_type_editor(comp.__class__)
            editor = editor_type(comp)
            child_editor.append(editor)
        for editor in child_editor:
            indent = max(indent, editor.get_indent())
        for editor in child_editor:
            editor.indent = indent
        return child_editor


@register_type_editor
class ComponentEditor(EditorBase):
    target_type = luna.Component

    def __init__(self, target):
        super().__init__(target)

    def on_category(self):
        comp_name = self.target.__class__.__name__
        category_name = "{}  {}".format(imgui.ICON_FA_LAYER_GROUP, comp_name)
        is_open = imgui.tree_node(id(self.target), imgui.ImGuiTreeNodeFlags_DefaultOpen, category_name)
        return is_open

    def on_imgui(self):
        target_type = self.target.__class__
        properties = target_type.get_properties()
        if self.on_category():
            for prop in properties:
                self.imgui_obj_property(self.target, prop.name, prop.type)
            imgui.tree_pop()
        imgui.separator()


@register_type_editor
class MeshRendererEditor(ComponentEditor):
    target_type = luna.MeshRenderer
    target: 'luna.MeshRenderer' = None

    def __init__(self, target):
        super().__init__(target)

    def get_indent(self):
        indent = super().get_indent()
        indent = max(indent, imgui.calc_text_size("Material Instance").x)
        return indent

    def on_imgui(self):
        mat_list: 'list[luna.MaterialInstance]' = self.target.material
        mesh: 'luna.ObjAsset' = self.target.mesh
        if self.on_category():
            self.imgui_obj_property(self.target, "mesh", luna.ObjAsset)
            self.imgui_obj_property(self.target, "material", luna.MaterialTemplateAsset)
            imgui.tree_pop()
