
function on_create()
    print('lua entry');
    local scene = luna.SceneManager.instance().main_scene;
    --local entity = scene.create_entity()
    --local comp = entity.add_component()

end

function tick(delta_time)
    print(delta_time);
end

on_create();