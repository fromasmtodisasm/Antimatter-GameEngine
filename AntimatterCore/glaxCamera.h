#pragma once
#include "SystemHeader.h"
#include "glaxTransform.h"

namespace Antimatter{
	namespace glx{

		class Camera
		{
		public:
			Camera()
			{
				target.Parent = &eye;
				//eye.Parent = &target;
				//target.LocalPosition = glm::vec3(0, 0, 0);
			};

			float Height;
			glx::Transform eye;
			glx::Transform target;
		};
	}
}

