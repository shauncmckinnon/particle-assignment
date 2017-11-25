/*
Shaun McKinnon - 100642799
Selina Daley   - 100558926
Dan Munusami   - 100552012
*/
#include "PointHandle.h"
#include "TTK\GraphicsUtils.h"

PointHandle::PointHandle(float _pointSize, glm::vec3 _position, std::string _label)
{
	pointSize = _pointSize;
	position = _position;
	label = _label;
}

bool PointHandle::isInside(glm::vec3 p)
{
	return glm::length((p - position)) < pointSize;
}

void PointHandle::draw()
{
	TTK::Graphics::DrawPoint(position, pointSize, glm::vec4(0.0f));
	TTK::Graphics::DrawText2D(label, position.x, position.y);
}
