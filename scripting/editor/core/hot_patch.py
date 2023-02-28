import importlib
import inspect
import os
import types

from watchdog.events import FileSystemEventHandler
from watchdog.observers import Observer


class FileModifyHandler(FileSystemEventHandler):
    def __init__(self, p):
        super().__init__()
        self.watch_path = p

    def on_modified(self, event):
        if "__pycache__" in event.src_path or event.src_path.endswith('~') or event.is_directory:
            return

        from editor.core.editor_module import EditorModule

        p = os.path.relpath(event.src_path, self.watch_path)
        module_name = p.replace("\\", ".")
        module_name = module_name.split(".py")[0]
        m = importlib.import_module(module_name)
        if m not in EditorModule.instance().reload_module:
            EditorModule.instance().reload_module.add(m)


def watch_scripts(dir_path):
    event_handler = FileModifyHandler(dir_path)
    observer = Observer()
    observer.schedule(event_handler, dir_path, recursive=True)
    observer.start()


def update_fun(old_fun, new_fun, update_cell_depth=2):
    old_cell_num = 0
    if old_fun.__closure__:
        old_cell_num = len(old_fun.__closure__)
    new_cell_num = 0
    if new_fun.__closure__:
        new_cell_num = len(new_fun.__closure__)

    if old_cell_num != new_cell_num:
        return False

    setattr(old_fun, '__code__', new_fun.__code__)
    setattr(old_fun, '__defaults__', new_fun.__defaults__)
    setattr(old_fun, '__doc__', new_fun.__doc__)
    setattr(old_fun, '__dict__', new_fun.__dict__)

    if not (update_cell_depth and old_cell_num):
        return True

    # for index, cell in enumerate(old_fun.__closure__):
    # 	if isinstance(cell, types.CellType):
    # 		update_fun(old_fun.__closure__[index], new_fun.__closure__[index], update_cell_depth - 1)

    return True


def update_type(old_class, new_class):
    for name, attr in old_class.__dict__.items():  # delete function
        if name in new_class.__dict__:
            continue
        type.__delattr__(old_class, name)

    for name, attr in new_class.__dict__.items():
        if name not in old_class.__dict__:  # new attribute
            setattr(old_class, name, attr)
            continue

        old_attr = old_class.__dict__[name]
        new_attr = attr

        if isinstance(old_attr, types.FunctionType) and isinstance(new_attr, types.FunctionType):
            if not update_fun(old_attr, new_attr):
                setattr(old_class, name, new_attr)


def reload_module(m):
    old_m = m
    members = inspect.getmembers(old_m, inspect.isclass)
    new_m = importlib.reload(old_m)
    old_cls_map = dict()
    for old_name, old_cls in members:
        old_cls_map[old_name] = old_cls
        if old_cls.__module__ == old_m.__name__:
            new_cls = new_m.__dict__[old_cls.__name__]
            update_type(old_cls, new_cls)

    new_members = inspect.getmembers(new_m, inspect.isclass)
    # attrs = dict(new_m.__dict__)
    for new_name, new_cls in new_members:
        if new_cls.__module__ == new_m.__name__:
            if new_name in old_cls_map:
                setattr(new_m, new_name, old_cls_map.get(new_name))
