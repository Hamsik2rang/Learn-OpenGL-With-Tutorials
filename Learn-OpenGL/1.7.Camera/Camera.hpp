#pragma once

#include <glad/glad.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
};

const float YAW			= -90.0f;
const float PITCH		= 0.0f;
const float SPEED		= 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV			= 45.0f;

class Camera
{
private:
	float _yaw			= YAW;
	float _pitch		= PITCH;
	float _speed		= SPEED;
	float _sensitivity	= SENSITIVITY;
	float _fov			= FOV;

	glm::vec3 _pos;
	glm::vec3 _forward;
	glm::vec3 _up;
	glm::vec3 _right;
	glm::vec3 _worldUp;

	void updateVector();
public:
	Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
		:_pos{ pos }, 
		_up{ up }, 
		_worldUp{ up },
		_yaw{ yaw }, 
		_pitch{ pitch }
	{
		updateVector();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
		:_forward{ glm::vec3(0.0f, 0.0f, -1.0f) }, 
		_speed{ SPEED }, 
		_sensitivity{ SENSITIVITY }, 
		_fov{ FOV }, 
		_pos{ glm::vec3(posX, posY, posZ) }, 
		_up{ glm::vec3(upX, upY, upZ) }, 
		_worldUp{glm::vec3(upX, upY, upZ)},
		_yaw{ YAW }, 
		_pitch{ PITCH }
	{
		updateVector();
	}

	glm::mat4 lookAt() { return glm::lookAt(_pos, _pos + _forward, _up); }

	void setForward(const glm::vec3& forward) { _forward = forward; }
	void setUp(const glm::vec3& up) { _up = up; }
	void setRight(const glm::vec3& right) { _right = right; }
	void setPosition(const glm::vec3& pos) { _pos = pos; }

	const glm::vec3 forward() const { return _forward; }
	const glm::vec3 up() const { return _up; }
	const glm::vec3 right() const { return _right; }
	const glm::vec3 position() const { return _pos; }
	const float fov() { return _fov; }

	void processKeyboard(CameraMovement direction, float deltaTime);
	void processMouseMovement(float xoffset, float yoffset, bool constrainPitch);
	void processMouseScroll(float yoffset);
};

void Camera::updateVector()
{
	glm::vec3 forward;
	forward.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	forward.y = sin(glm::radians(_pitch));
	forward.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

	// is normalize necessary?
	_forward = glm::normalize(forward);
	_right = glm::normalize(glm::cross(_forward, _worldUp));
	_up = glm::normalize(glm::cross(_right, _forward));
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = _speed * deltaTime;
	switch (direction)
	{
	case CameraMovement::FORWARD:
		_pos += _forward * velocity;
		break;
	case CameraMovement::BACKWARD:
		_pos -= _forward * velocity;
		break;
	case CameraMovement::RIGHT:
		_pos += _right * velocity;
		break;
	case CameraMovement::LEFT:
		_pos -= _right * velocity;
		break;
	}
}

void Camera::processMouseMovement(float xoffset, 
	float yoffset,
	bool constrainPitch = true)
{
	xoffset *= _sensitivity;
	yoffset *= _sensitivity;

	_yaw += xoffset;
	// add negative yoffset to implement FPS-style camera
	_pitch -= yoffset;

	if (constrainPitch)
	{
		_pitch = _pitch > 89.0f ? 89.0f : _pitch < -89.0f ? -89.0f : _pitch;
	}

	updateVector();
}

void Camera::processMouseScroll(float yoffset)
{
	_fov -= yoffset;
	_fov = _fov > FOV ? FOV : _fov < 1.0f ? 1.0f : _fov;
}