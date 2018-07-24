#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Transform.h"
#include "glaxCamera.h"

using namespace System::Drawing;

namespace Antimatter{
	namespace Games
	{
		public ref class Camera
		{

		public:

			float Height;
			Vector^ offset;
			Camera()
			{
				eye = gcnew Transform();
				target = gcnew Transform();
				eye->Position = gcnew Vector(0.f, 0.f, -5.f);
				offset = gcnew Vector(0.f, 0.f, -5.f);
				target->Position = eye->Position + offset;
				target->SetParent(eye);
			}

			void DebugNativeCamera()
			{
				cout<<"-> Camera Native eye: "<< "X: "<< eye->gxtransform->WorldPosition.x << " Y: "<< eye->gxtransform->WorldPosition.y << " Z: " << eye->gxtransform->WorldPosition.z<<endl;
			}

			Transform^ eye;
			Transform^ target;
		};
	}
}

