import luna


# Test Luna
def test_luna():
	i = luna.LVector3f(0, 0, 0)
	v = luna.LVector3f(1, 1, 1)
	p = i + v

	print(p.x)
	print(p.y)
	print(p.z)

	props = luna.Transform.get_properties()

	for prop in props:
		print(prop.name)
		print(prop.type)