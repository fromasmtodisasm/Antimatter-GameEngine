#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Quaternion.h"
#include "glaxTransform.h"

//http://forum.unity3d.com/threads/understanding-rotations-in-local-and-world-space-quaternions.153330/

//world to local
//https://gamedev.stackexchange.com/questions/34076/global-transform-to-local-transform
//extracting matrix
//https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati
//extracting coords
//https://www.gamedev.net/forums/topic/653507-glm-matrix-layout/

//implementing cli dispose
//https://msdn.microsoft.com/en-us/library/ms177197%28v=vs.90%29.aspx?f=255&MSPPError=-2147217396
//https://msdn.microsoft.com/en-us/library/ms177197%28v=vs.90%29.aspx?f=255&MSPPError=-2147217396

namespace Antimatter {
	namespace Games {

		public ref class Transform
		{
		protected:
			!Transform()
			{
				//deleting the transform used to cause a crash
				//ensure that the native transform is free for deletion
				delete gxtransform;
				gxtransform = NULL;
			}

		private:
			Vector^ eulerAngles;
			Vector^ localEulerAngles;

		public:
			String^ name = "Transform";
			Transform^ parent;
			List<Transform^>^ children;
			glx::Transform* gxtransform;
			
			~Transform()
			{
				//should hopefully no longer crash
				this->!Transform();
			}

			Transform()
			{
				gxtransform = new glx::Transform();
				Position = gcnew Vector();
				EulerAngles = gcnew Vector();
				Rotation = gcnew Quaternion();
				Scale = gcnew Vector(1.f);
				LocalEulerAngles = gcnew Vector();
				children = gcnew List<Transform^>();
			}

			static Transform^ Clone(Transform^ t)
			{
				Transform^ trans = gcnew Transform();
				trans->Position = t->Position;
				trans->EulerAngles = t->eulerAngles;
				trans->Rotation = t->Rotation;
				trans->Scale = t->Scale;
				return trans;
				//implement deep copy when necessary
				//trans->children = gcnew List<Transform^>();
			}

			glm::mat4* glm()
			{
				return &gxtransform->staticMatrix;
			}

			glm::mat4 Inverse()
			{
				return glm::inverse(gxtransform->staticMatrix);
			}

			static Vector^ operator*(Transform^ trans, Vector^ vec)
			{
				return gcnew Vector(trans->gxtransform->staticMatrix * vec);
			}

			static Vector^ operator*( Vector^ vec, Transform^ trans)
			{
				return glm::inverse(trans->gxtransform->staticMatrix)*vec;
			}
			
			static Vector^ operator*(glm::vec4 v4, Transform^ trans)
			{
				return gcnew Vector(v4*trans->gxtransform->GetWorldMatrix());
			}

			static Vector^ operator*(Transform^ trans, glm::vec4 v4)
			{
				return gcnew Vector(trans->gxtransform->GetWorldMatrix()*v4);
			}

			property  bool IsStatic
			{
				bool get()
				{
					return gxtransform->isStatic;
				}
				void set(bool value) { gxtransform->isStatic = value; }
			};

			property Vector ^Position
			{
				Vector^ get()
				{
					return glm::vec3(gxtransform->GetPosition()); 
				}
				void set(Vector^ vec){ gxtransform->SetPosition(vec->glm()); }
			};

			property Quaternion^Rotation
			{
				Quaternion^ get(){ return gcnew Quaternion(gxtransform->GetRotation()); }
				void set(Quaternion^ quat){ gxtransform->SetRotation(quat->glm()); }
			};

			property Vector ^Scale
			{
				Vector^ get(){ return glm::vec3(gxtransform->GetScale()); }
				void set(Vector^ vec){ gxtransform->SetScale(vec->glm()); }
			};

			property Vector ^EulerAngles
			{
				Vector^ get()
				{
					return eulerAngles;
				}
				void set(Vector^ vec)
				{
					eulerAngles = vec;
					glm::vec3 EA(math_deg *vec->x, math_deg *vec->y, math_deg *vec->z);
					gxtransform->SetRotation(glm::quat(EA));
				}
			};

			property Vector ^LocalEulerAngles
			{
				Vector^ get()
				{
					return localEulerAngles;
				}
				void set(Vector^ vec)
				{
					localEulerAngles = vec;
					glm::vec3 EA(math_deg *vec->x, math_deg *vec->y, math_deg *vec->z);
					gxtransform->SetLocalRotation(glm::quat(EA));
				}
			};

			property Vector ^LocalPosition
			{
				Vector^ get(){ return glm::vec3(gxtransform->GetLocalPosition()); }
				void set(Vector^ vec){ gxtransform->SetLocalPosition(vec->glm()); }
			};

			property Quaternion^LocalRotation
			{
				Quaternion^ get(){ return gcnew Quaternion(gxtransform->GetLocalRotation()); }
				void set(Quaternion^ quat){ gxtransform->SetLocalRotation(quat->glm()); }
			};

			property Vector ^LocalScale
			{
				Vector^ get(){ return glm::vec3(gxtransform->GetLocalScale()); }
				void set(Vector^ vec){ gxtransform->SetLocalScale(vec->glm()); }
			};

			property Transform^ Parent
			{
				void set(Transform^ trans)
				{
					SetParent(trans);
				};

				Transform^ get()
				{
					return parent;
				};
			};

			void SetParent(Transform^ trans)
			{
				if (trans == nullptr)
				{
					parent = nullptr;
					gxtransform->Parent = nullptr;
					return;
				}
				LocalPosition = trans->Position - Position; 
				LocalRotation = gcnew Quaternion(gxtransform->WorldRotation* glm::conjugate(trans->gxtransform->WorldRotation));
				glm::vec3 s = trans->gxtransform->WorldScale;
				LocalScale = gxtransform->WorldScale *(glm::vec3(1.f / s.x, 1.f / s.y, 1.f / s.z));
				parent = trans;
				gxtransform->Parent = trans->gxtransform;
				
				//glm::vec4 localPos = parent->gxtransform->GetWorldMatrix()*Position->ToGLMvec4();
				//Position = glm::vec3(localPos);
			}

			static Quaternion^ CreateEulerAngles(Vector^ vec)
			{
				glm::vec3 EA(math_deg *vec->x, math_deg *vec->y, math_deg *vec->z);
				return gcnew Quaternion(glm::quat(EA));
			}

			void SetDirection(Vector^ axis, Vector^ direction)
			{
				gxtransform->SetRotation(gxtransform->SetDirection(axis->glm(), direction->glm()));
			}

			Vector^ GetDirection(float x, float y, float z)
			{
				return gcnew Vector(gxtransform->GetDirection(x,y,z));
			}

			Vector^ GetDirection(Vector^ vec)
			{
				return GetDirection(vec->x, vec->y,vec->z);
			}

			property Vector ^ ForwardDirection
			{
				Vector^ get()
				{ 
					return GetDirection(0, 0, 1);
				}
				void set(Vector^ vec){  }
			};

			property Vector ^ UpDirection
			{
				Vector^ get()
				{
					return GetDirection(0, 1, 0);
				}
				void set(Vector^ vec){  }
			};

			property Vector ^ RightDirection
			{
				Vector^ get()
				{
					return GetDirection(1, 0, 0);
				}
				void set(Vector^ vec){  }
			};

			property Vector ^ Forward
			{
				Vector^ get()
				{
					return Position + GetDirection(0, 0, 1);
				}
				void set(Vector^ vec){  }
			};

			property Vector ^ Up
			{
				Vector^ get()
				{
					return Position + GetDirection(0, 1, 0);
				}
				void set(Vector^ vec){  }
			};

			property Vector ^ Right
			{
				Vector^ get()
				{
					return Position + GetDirection(1, 0, 0);
				}
				void set(Vector^ vec){  }
			};

			cli::array<Vector^>^ TransformedArray(cli::array<Vector^>^ vectorArray)
			{
				List<Vector^>^ VL = gcnew List<Vector^>();
				for (int i = 0; i < vectorArray->Length; i++)
				{
					//TODO  we can set the w value here to reduce function call
					VL->Add(this*vectorArray[i]->W(1.f));
				}

				return VL->ToArray();
			}
		};

		public ref class Pair
		{
		public:
			Transform^ a;
			Transform^ b;

			Pair()
			{

			}

			Pair(Transform^ A, Transform^ B)
			{
				a = A;
				b = B;
			}

			bool Equals(Transform^ A, Transform^ B)
			{
				if ((a == A || B) && B == A||B)
				{
					return true;
				}
				return false;
			}

			bool Contains(Transform^ t)
			{
				if (t == a || b)
					return true;
				return false;
			}

			String^ GetString()
			{
				return String::Concat(a->name, " + ", b->name, " ");
			}

			static bool operator==(Pair^ A, Pair^ B)
			{
				return A->Equals(B);
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

