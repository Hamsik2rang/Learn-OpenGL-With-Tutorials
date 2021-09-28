#pragma once
#include "Shader.hpp"

#include <random>
#include <functional>

namespace Hsrang
{
	inline namespace Utility
	{
		glm::vec3 getRandomPosition()
		{
			std::random_device rd;
			std::mt19937 mersenne(rd());
			std::uniform_real_distribution<> generator(-80.0, 80.0);

			float elem[3]{ 0.0f };
			for (int i = 0; i < 3; i++)
			{
				elem[i] = (float)generator(mersenne);
			}

			return glm::vec3(elem[0], elem[1], elem[2]);
		}
	}
}
