import os
import sys

from main import init_editor

work_dir = os.getcwd()
bin_dir = work_dir + "\\bin\\Debug"
editor_dir = work_dir + "\\editor"

if __name__ == '__main__':
    sys.path.append(bin_dir)
    import luna
    luna.add_library_dir(bin_dir)
    is_looping = True
    init_editor()

