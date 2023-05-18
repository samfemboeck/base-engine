#pragma once

#include <glm/glm.hpp>
#include "QuickMaths.h"

class Camera
{
public:
	static Vec2f screenToWorldPoint(const Vec2f&, const glm::mat4&);

public:
	void setOrtho(float left, float right, float bottom, float top);
	void setPerspective(float fov, float aspect, float nearPlane, float farPlane);

	const glm::mat4& getProjectionMatrix() const { return projectionMatrix_; }

private:
	glm::mat4 projectionMatrix_;
};
