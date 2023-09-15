#include "Camera.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

Camera::Camera() : location(0.f)
{
	pitch = 0.f;
	yaw = -90.f;
	roll = 0;
	WorldUp = vec3(0.0f, 1.0f, 0.0f);
	updateViewMatrix();
}

const glm::mat4& Camera::GetViewMatrix()
{
	return view; // could be inline
}

// ROLL NOT IMPLEMENTED
void Camera::updateViewMatrix()
{
	vec3 _front;
	_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	_front.y = sin(glm::radians(pitch));
	_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(_front);

	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));

	view = lookAt(location, location + front, up);
}

void Camera::AddMovementInput(const glm::vec3& direction)
{
	const vec3 mov = direction * cameraSpeed;
	SetLocation(mov + location);
}

void Camera::SetLocation(const glm::vec3& location)
{
	this->location = location;
	// update front, up and right
	updateViewMatrix();
}

void Camera::SetRotation(const float& pitch, const float& yaw, const float roll)
{
	this->pitch = pitch; this->yaw = yaw; this->roll = roll;
	updateViewMatrix();
}

void Camera::SetRotation(const glm::vec3& rotation)
{
	pitch = rotation.x, yaw = rotation.y, roll = rotation.z;
	updateViewMatrix();
}

float MouseSensitivity = 0.1f;
void Camera::onCursorMove(vec2 delta)
{
	
	delta.x *= MouseSensitivity;
	delta.y *= MouseSensitivity;

	yaw += delta.x;
	pitch += delta.y;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.f)
		pitch = -89.f;

	updateViewMatrix();
}
