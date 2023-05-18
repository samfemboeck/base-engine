#pragma once

#include "Camera.h"

class OrthoCamController
{
public:
	static OrthoCamController& getInstance();
	OrthoCamController() = default;

	void update();
	void setDimensions(float width, float aspect);
	bool scrollMouse(double mouseOffsetY);
	float getZoomLevel();
	inline glm::mat4 getViewProjection() { return getProjection() * getView(); }
	inline glm::mat4 getView() { return glm::inverse(glm::translate(glm::mat4(1), position_)); }
	inline glm::mat4 getProjection() { return camera_.getProjectionMatrix(); }
	inline Camera& getCamera() { return camera_; }
	void setPosition(Vec2f pos);
	Vec2f getPosition();

private:
	Camera camera_;
	float aspectRatio_ = 0;
	float width_ = 0;
	float zoomLevel_ = 0.833575f;
	glm::vec3 position_ = glm::vec3{ 0.0f, -275.0f, 0.0f };
	float speedTranslation_ = 5.0f;
	float speedScroll_ = 0.25f;
};
