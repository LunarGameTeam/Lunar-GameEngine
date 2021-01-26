
function on_create()
    print('lua entry');
    local scene = luna.SceneManager.instance().main_scene;
end

function tick(delta_time)
    print(delta_time);
end

on_create();