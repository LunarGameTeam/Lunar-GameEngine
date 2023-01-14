import os


class FileInfo(object):
    def __init__(self, file_path):
        self.abs_path = file_path
        from core.editor_module import platform_module
        self.path = os.path.relpath(file_path, platform_module.engine_dir)
        self.name = os.path.basename(file_path)


class FolderInfo(FileInfo):
    def __init__(self, dir_path):
        super().__init__(dir_path)
        self.child_list = []
        self.init()

    def init(self):
        for f in os.listdir(self.abs_path):
            abs_path = os.path.join(self.abs_path, f)
            if os.path.isdir(abs_path):
                d = FolderInfo(abs_path)
                self.child_list.append(d)
            elif os.path.isfile(abs_path):
                file = FileInfo(abs_path)
                self.child_list.append(file)
