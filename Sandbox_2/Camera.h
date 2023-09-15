#pragma once

#include <glm/glm.hpp>

class Camera
{
	glm::vec3 location;
	glm::vec3 front, up, right;
	glm::mat4 view;
	glm::vec3 WorldUp;
	
public:
	float pitch, yaw, roll;
	void updateViewMatrix();

public:
	Camera();
	const glm::mat4& GetViewMatrix();
	inline glm::vec3 GetLocation() { return location; }
	inline glm::vec3 GetFrontVector() { return front; }
	inline glm::vec3 GetUpVector() { return up; }
	inline glm::vec3 GetRightVector() { return right; }

	void AddMovementInput(const glm::vec3& direction);
	//void AddRotationInput()

	void SetLocation(const glm::vec3& location);
	void SetRotation(const float& pitch, const float& yaw, const float roll);
	void SetRotation(const glm::vec3& rotation);

	float cameraSpeed = 5.f;

public:
	void onCursorMove(glm::vec2 delta);
};

