import os
from typing import Type

from luna import imgui

asset_types = {}


def register_asset_type(cls):
    for suffix in cls.suffix:
        if suffix not in asset_types:
            asset_types[suffix] = cls()
        else:
            assert (False)


class AssetType(object):
    suffix = []
    icon = imgui.ICON_FA_FILE

    @staticmethod
    def on_double_click(item: 'FileInfo'):
        pass


class FolderAsset(AssetType):
    suffix = []
    icon = imgui.ICON_FA_FOLDER


def get_asset_type(name) -> type[AssetType]:
    for asset_suffix, asset_type in asset_types.items():
        if name.endswith(asset_suffix):
            return asset_type
    return AssetType


@register_asset_type
class TextureAssetType(AssetType):
    suffix = [".png", ".dds"]
    icon = imgui.ICON_FA_FILE_IMAGE

    @staticmethod
    def on_double_click(item):
        try:
            import os
            os.startfile(item.abs_path)
        except:
            from editor.core.editor_module import EditorModule
            EditorModule.instance().main_scene_window.show_status("没有应用程序关联此类型的文件")


class FileInfo(object):
    asset_type: Type[AssetType]

    def __init__(self, file_path):
        self.abs_path = file_path
        from editor.core.editor_module import platform_module
        if file_path.startswith(platform_module.project_dir):
            self.path = os.path.relpath(file_path, platform_module.project_dir)
        else:
            self.path = os.path.relpath(file_path, platform_module.engine_dir)

        self.path = self.path.replace('\\', '/')

        self.name = os.path.basename(file_path)
        self.asset_type = get_asset_type(self.name)


class FolderInfo(FileInfo):

    def __init__(self, dir_path):
        super().__init__(dir_path)
        self.asset_type = FolderAsset
        self.child_list = []
        self.init()

    def init(self):
        for f in os.listdir(self.abs_path):
            abs_path = self.abs_path + os.sep + f
            if os.path.isdir(abs_path):
                d = FolderInfo(abs_path)
                self.child_list.append(d)
            elif os.path.isfile(abs_path):
                file = FileInfo(abs_path)
                self.child_list.append(file)
