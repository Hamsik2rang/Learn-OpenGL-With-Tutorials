#pragma once
#include "Shader.hpp"
#include "Constant.h"

namespace Hsrang
{
	class Camera
	{
	private:
		float _fov;
		float _speed;
		float _sensitivity = 0.1f;
		bool _isFirstMove;

		glm::vec3 _pos;
		glm::vec3 _forward;
		glm::vec3 _up;
		glm::vec3 _right;

		float _pitch;
		float _yaw;
		float _roll;
	public:
		Camera(float, float, float);

		void move(int, int, float);
		void rotate(double, double);
		void zoom(double);

		float fov() const;
		glm::vec3 pos() const;
		glm::vec3 forward() const;
		glm::vec3 up() const;
		glm::vec3 right() const;
	};

	Camera::Camera(float xpos = 0.0f, float ypos = 0.0f, float zpos = 3.0f)
		:_fov{ 45.0f }, _speed{ 2.0f }, _sensitivity{ 0.1f }, _isFirstMove{ true },
		
		_pitch{ 0.0f }, _yaw{ -90.0f }, _roll{ 0.0f }
	{
		_pos = glm::vec3(xpos, ypos, zpos);

		_forward = glm::normalize(glm::vec3(_pos.x, _pos.y, _pos.z - 3.0f) - _pos);
		_right = glm::cross(_forward, glm::vec3(0.0f, 1.0f, 0.0f));
		_up = glm::cross(_right, _forward);
	}

	void Camera::move(int forward, int right, float dt)
	{
		if (forward > 0)
		{
			_pos += _forward * dt * _speed;
		}
		else if (forward < 0)
		{
			_pos -= _forward * dt * _speed;
		}

		if (right > 0)
		{
			_pos += _right * dt * _speed;
		}
		else if (right < 0)
		{
			_pos -= _right * dt * _speed;
		}
	}

	void Camera::rotate(double xpos, double ypos)
	{
		static double lastX, lastY;

		if (_isFirstMove)
		{
			lastX = xpos;
			lastY = ypos;
			_isFirstMove = false;
		}

		auto dx = (float)(xpos - lastX) * _sensitivity;
		auto dy = (float)(ypos - lastY) * _sensitivity;
		lastX = xpos;
		lastY = ypos;

		_yaw += dx;
		_pitch -= dy;

		_pitch = _pitch > 89.0f ? 89.0f : _pitch < -89.0f ? -89.0f : _pitch;


		glm::vec3 forward;
		forward.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		forward.y = sin(glm::radians(_pitch));
		forward.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

		_forward = glm::normalize(forward);
		_right = glm::cross(_forward, glm::vec3(0.0f, 1.0f, 0.0f));
		_up = glm::cross(_right, _forward);
	}
	
	void Camera::zoom(double dy)
	{
		_fov -= (float)dy;
		_fov = _fov > 45.0f ? 45.0f : _fov < 1.0f ? 1.0f : _fov;
		std::cout << _fov << std::endl;
	}

	float Camera::fov() const
	{
		return _fov;
	}

	glm::vec3 Camera::pos() const
	{
		return _pos;
	}

	glm::vec3 Camera::forward() const
	{
		return _forward;
	}

	glm::vec3 Camera::up() const
	{
		return _up;
	}

	glm::vec3 Camera::right() const
	{
		return _right;
	}
}