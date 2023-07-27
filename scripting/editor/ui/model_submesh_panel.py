import luna
from luna import imgui
from editor.ui.panel import PanelBase

class ModelSubmeshPanel(PanelBase):
    def __init__(self) -> None:
        super().__init__()
        self.scene = None
        self.title = "submesh"
        self.selected_entity = None

    def on_menu(self):
        pass

    def on_imgui(self, delta_time) -> None:
        super().on_imgui(delta_time)
        self.on_menu()
        submesh_test = ["submesh0","submesh1"]

        for idx in range(0, 2):
            flag = 1 << 8 | 1 << 5
            clicked, expand, double_click = imgui.tree_node_callback(id(submesh_test[idx]), luna.LVector2f(-1, 32), flag)
            imgui.text("{} {}".format(imgui.ICON_FA_CUBE, submesh_test[idx]))
            if expand:
                imgui.tree_pop()
            if clicked:
                self.on_entity_clicked(idx)

    def on_entity_clicked(self, idx):
        pass