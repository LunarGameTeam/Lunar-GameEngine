def binding_test():
    import luna
    vec3f_1 = luna.LVector3f(0, 0, 0)
    vec3f_2 = luna.LVector3f(1, 1, 1)
    result = vec3f_1 + vec3f_2

    quaterion1 = luna.LQuaternion(1, 1, 1, 1)
    quaterion2 = luna.LQuaternion(1, 1, 1, 1)

    assert (result.x == vec3f_1.x + vec3f_2.x)
    assert (result.y == vec3f_1.y + vec3f_2.y)
    assert (result.z == vec3f_1.z + vec3f_2.z)

    del result
    luna.print("Math Test Passed")

    props = luna.Transform.get_properties()
    assert (len(props) != 0)
    for prop in props:
        luna.print(prop.name)
        luna.print(prop.type)
    del props
    luna.print("Type Test Passed")
