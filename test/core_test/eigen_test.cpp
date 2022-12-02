#include "core/math/math.h"
#include <gtest/gtest.h>

using namespace luna;

#undef main


TEST(Core, Math)
{
	LVector3f pos(1, 1, 1);
	LQuaternion rotation = LQuaternion::Identity();
	LVector3f scale(0, 0, 0);
	LTransform t = LTransform::Identity();
	t.translate(pos);
	t.rotate(rotation);
	t.scale(1.f);
	t = t.inverse();
	auto& mat = t.matrix();	
	LVector4f p2 = mat * LVector4f(1,1,1,1);
	
}