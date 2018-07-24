#pragma once

#include "SystemHeader.h"
#include "GlaxionHeader.h"

//http://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php
//http://stackoverflow.com/questions/13014973/quaternion-rotate-to

//getting euler angles from quaternion
//http://www.tinkerforge.com/en/doc/Software/Bricks/IMU_Brick_CSharp.html
/*
roll  = Mathf.Atan2(2*y*w - 2*x*z, 1 - 2*y*y - 2*z*z);
pitch = Mathf.Atan2(2*x*w - 2*y*z, 1 - 2*x*x - 2*z*z);
yaw   =  Mathf.Asin(2*x*y + 2*z*w);

yaw   =  atan2(2*x*y + 2*w*z, w*w + x*x - y*y - z*z)
pitch = -asin(2*w*y - 2*x*z)
roll  = -atan2(2*y*z + 2*w*x, -w*w + x*x + y*y - z*z))

*/
//http://answers.unity3d.com/questions/525952/how-i-can-converting-a-quaternion-to-a-direction-v.html

//http://stackoverflow.com/questions/15873996/converting-a-direction-vector-to-a-quaternion-rotation

namespace Antimatter
{
	namespace glx
	{
		class Transform
		{
		public:

			Transform()
			{
				WorldPosition = glm::vec3(0, 0, 0);
				WorldRotation = glm::quat(0, 0, 0, 1);
				WorldScale = glm::vec3(1, 1, 1);
				LocalPosition = glm::vec3(0, 0, 0);
				LocalRotation = glm::quat(0, 0, 0, 1);
				LocalScale = glm::vec3(1, 1, 1);
				rotationMatrix = glm::mat4(1.f);
				Parent = NULL;
				isStatic = false;
				staticMatrix = glm::mat4(1.f);
			};
			~Transform() {};

			glm::vec3 WorldPosition;
			glm::quat WorldRotation;
			glm::vec3 WorldScale;
			glx::Transform* Parent;
			std::vector<glx::Transform*> Children;	 //needs initializing?
			glm::vec3 LocalPosition;
			glm::quat LocalRotation;
			glm::vec3 LocalScale;
			glm::mat4 WorldMatrix;
			glm::mat4 staticMatrix;
			glm::mat4 rotationMatrix;
			glm::mat4 pointTransformMatrix;
			bool isStatic;
			glm::mat4 TranslationMatrix;

			void SetPosition(glm::vec3 value);

			//WorldPosition
			glm::vec3 GetPosition();

			glm::quat SetDirection(glm::vec3 axis, glm::vec3 direction);

			void SetRotation(glm::quat value);

			glm::quat GetRotation();

			void SetScale(glm::vec3 value);

			glm::vec3 GetScale();

			void SetLocalPosition(glm::vec3 value);

			glm::vec3 GetLocalPosition();

			void SetLocalRotation(glm::quat value);

			glm::quat GetLocalRotation();

			void SetLocalScale(glm::vec3 value);

			glm::vec3 GetLocalScale();

			glm::mat4 GetWorldMatrix();

			glm::mat4 GetTranslationMatrix();

			glm::vec3 GetDirection(float x, float y, float z)
			{
				return GetRotation()*glm::vec3(x, y, z);
			}

			glm::vec3 GetDirection(glm::vec3 vec)
			{
				return GetRotation()*vec;
			}

			glm::vec3 GetForwardDirection()
			{
				return GetDirection(0, 0, 1);
			}
			glm::vec3 GetUpDirection()
			{
				return GetDirection(0.f, 1.f, 0.f);
			}
			glm::vec3 GetRightDirection()
			{
				return GetDirection(1.f, 0.f, 0.f);
			}

			glm::vec3 GetForward()
			{
				return GetPosition() + GetDirection(0.f, 0.f, 1.f);
			}
			glm::vec3 GetUp()
			{
				return GetPosition() + GetDirection(0.f, 1.f, 0.f);
			}
			glm::vec3 GetRight()
			{
				return GetPosition() + GetDirection(1.f, 0.f, 0.f);
			}

			void coutglm(const char * text,glm::vec3 vec)
			{
				cout << text << "\n";
				cout << "	X: " << vec.x << "  Y: " << vec.y << "  Z: " << vec.z<<"\n";
			}

			void coutglm(const char * text, glm::quat vec)
			{
				cout << text << "\n";
				cout << "	X: " << vec.x << "  Y: " << vec.y << "  Z: " << vec.z <<"  W: "<<vec.w<<"\n";
			}

			void Debug(const char* text)
			{
				cout <<"\n"<< text << "\n";
				coutglm("WorldPosition: ", GetPosition());
				coutglm("WorldRotation: ", GetRotation());
				coutglm("WorldScale: ", GetScale());

				coutglm("LocalPosition: ", GetLocalPosition());
				coutglm("LocalRotation: ", GetLocalRotation());
				coutglm("LocalScale: ", GetLocalScale());
				if(Parent)
				{
					cout << "\nParent\n";
					coutglm("Parent WorldPosition: ", Parent->GetPosition());
					coutglm("Parent WorldRotation: ", Parent->GetRotation());
					coutglm("Parent WorldScale: ", Parent->GetScale());

					coutglm("Parent LocalPosition: ", Parent->GetLocalPosition());
					coutglm("Parent LocalRotation: ", Parent->GetLocalRotation());
					coutglm("Parent LocalScale: ", Parent->GetLocalScale());
				}
				else{
					cout << "\nNo Parent\n";
				}
			}
		};
	}
}

/*
class tTransform
{

// data
tVector4f    m_Position;
tQuaternion  m_Rotation;
tVector4f    m_Scale;

public:
// World = Parent * Local (*this == parent)
tTransform operator * (const tTransform& localSpace)
{
tTransform worldSpace;
worldSpace.m_Position = m_Position +
m_Rotation * (localSpace.m_Position * m_Scale);
worldSpace.m_Rotation = m_Rotation * localSpace.m_Rotation;
worldSpace.m_Scale = m_Scale * (m_Rotation * localSpace.m_Scale);
return worldSpace;
}

// Local = World / Parent (*this = World)
tTransform operator / (const tTransform& parentSpace)
{
tTransform localSpace;
tQuaternion parentSpaceConjugate = parentSpace.m_Rotation.conjugate();
localSpace.m_Position = (parentSpaceConjugate *
(m_Position - parentSpace.m_Position)) /
parentSpace.m_Scale;

localSpace.m_Rotation = parentSpaceConjugate * m_Rotation;

localSpace.m_Scale = parentSpaceConjugate *
(m_Scale / parentSpace.m_Scale);
return localSpace;
}
};
*/

