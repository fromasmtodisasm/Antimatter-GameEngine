#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Vector.h"

//http://www.brendonclifford.net/quaternion-c.html
//https://cs.stanford.edu/~acoates/quaternion.h

namespace Antimatter {
	namespace Games {

		public ref struct Quaternion
		{
		public:

			float x;
			float y;
			float z;
			float w;
			static Quaternion^ identity = gcnew Quaternion(0.f, 0.f, 0.f, 1.f);

			Quaternion()
			{
				x = 0.f;
				y = 0.f;
				z = 0.f;
				w = 1.f;
			};

			Quaternion(float X, float Y, float Z, float W)
			{
				x = X;
				y = Y;
				z = Z;
				w = W;
			};

			Quaternion(Vector^ v)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				w = 0.f;
			};

			Quaternion(Vector^ v, float angle)
			{

				x = v->x;
				y = v->y;
				z = v->z;
				w = angle;
			};

			Quaternion(glm::quat quat)
			{
				this->x = quat.x;
				this->y = quat.y;
				this->z = quat.z;
				this->w = quat.w;
			};

			Quaternion(glm::mat4 matrix)
			{
				glm::quat quat = glm::quat(matrix);
				this->x = quat.x;
				this->y = quat.y;
				this->z = quat.z;
				this->w = quat.w;
			};

			Quaternion^ Conjugate()
			{
				return gcnew Quaternion(-this->x, -this->y, -this->z, this->w);
			}

			Quaternion^ inverse()
			{
				return gcnew Quaternion(-x, -y, -z, w);
			}

			static property Quaternion^ Identity
			{
				Quaternion^ get() {
					return gcnew Quaternion();
				};
			}

			glm::mat4 ToMatrix()
			{
				return glm::mat4(glm::quat(this));
			}

			glm::quat glm()
			{
				return glm::quat(this);
			}

			String^ ToString() override
			{
				return String::Concat("Quaternion:\nX: ", x, " Y: ", y, " Z: ", z, " W: ", w);
			}

			void Print()
			{
				cout<<"Quaternion:\nX: "<< x<< " Y: "<< y<< " Z: "<< z<< " W: "<< w<<endl;
			}

			static Quaternion^ operator*(Quaternion^ quaternion, float scale)
			{
				return Quaternion::Multiply(quaternion, scale);
			}

			static Quaternion^ operator/(Quaternion^ quaternion, float scale)
			{
				float division = 1.f / scale;
				return quaternion*division;
			}

			static Quaternion^ Divide(Quaternion^ quaternion, float scale)
			{
				return quaternion / scale;
			}

			static Quaternion^ Multiply(Quaternion^ quaternion1, float scaleFactor)
			{
				Quaternion^ quaternion = gcnew Quaternion();
				quaternion->x = quaternion1->x * scaleFactor;
				quaternion->y = quaternion1->y * scaleFactor;
				quaternion->z = quaternion1->z * scaleFactor;
				quaternion->w = quaternion1->w * scaleFactor;
				return quaternion;
			}

			static float LengthSquared(Quaternion^ quaternion)
			{
				return (((quaternion->x * quaternion->x) + (quaternion->y * quaternion->y)) + (quaternion->z * quaternion->z)) + (quaternion->w * quaternion->w);
			}

			static float Length(Quaternion^ quaternion)
			{
				return sqrt(Quaternion::LengthSquared(quaternion));
			}

			float Length()
			{
				return Quaternion::Length(this);
			}

			float LengthSquared()
			{
				return Quaternion::LengthSquared(this);
			}

			static Quaternion^ Normalize(Quaternion^ quaternion)
			{
				return gcnew Quaternion(glm::normalize(quaternion->glm()));
			}

			Quaternion^ Normalize()
			{
				return gcnew Quaternion(glm::normalize(this->glm()));
			}

			void FromEulerAngles(float x, float y, float z)
			{
				glm::quat q = glm::quat(glm::vec3(math_deg *x, math_deg *y, math_deg *z));
				this->x = q.x;
				this->y = q.y;
				this->z = q.z;
				this->w = q.w;
			}

			void FromEulerAngles(Vector^ vec)
			{
				FromEulerAngles(vec->x, vec->y, vec->z);
			}

			static operator Quaternion^(Vector^ vec)
			{
				return gcnew Quaternion(glm::quat(glm::vec3(math_deg *vec->x, math_deg *vec->y, math_deg *vec->z)));
			}

			static operator glm::quat(Quaternion^ quaternion)
			{
				glm::quat quat;
				quat.x = quaternion->x;
				quat.y = quaternion->y;
				quat.z = quaternion->z;
				quat.w = quaternion->w;
				return quat;
			}

			Vector^ RotateVector(Vector^ vec)
			{
				Quaternion^ qv = gcnew Quaternion(vec->x, vec->y, vec->z, 0.f);
				Quaternion^ g = this * qv * -this;
				return gcnew Vector(g->x, g->y, g->z);
			}

			//unity implementation
			//http://answers.unity3d.com/questions/372371/multiply-quaternion-by-vector3-how-is-done.html
			static Vector^ operator *(Quaternion^ quat, Vector^ vec) 
			{
				float num = quat->x * 2.f;
				float num2 = quat->y * 2.f;
				float num3 = quat->z * 2.f;
				float num4 = quat->x * num;
				float num5 = quat->y * num2;
				float num6 = quat->z * num3;
				float num7 = quat->x * num2;
				float num8 = quat->x * num3;
				float num9 = quat->y * num3;
				float num10 = quat->w * num;
				float num11 = quat->w * num2;
				float num12 = quat->w * num3;
				Vector^ result = gcnew Vector();
				result->x = (1.f - (num5 + num6)) * vec->x + (num7 - num12) * vec->y + (num8 + num11) * vec->z;
				result->y = (num7 + num12) * vec->x + (1.f - (num4 + num6)) * vec->y + (num9 - num10) * vec->z;
				result->z = (num8 - num11) * vec->x + (num9 + num10) * vec->y + (1.f - (num4 + num5)) * vec->z;
				return result;
			}

			static Quaternion^ operator*(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				Quaternion^ q = gcnew Quaternion(quaternion1->glm()*quaternion2->glm());
				return q;
			}

			static Quaternion^ operator +(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				Quaternion^ quaternion = gcnew Quaternion();
				quaternion->x = quaternion->x + quaternion2->x;
				quaternion->y = quaternion1->y + quaternion2->y;
				quaternion->z = quaternion1->z + quaternion2->z;
				quaternion->w = quaternion1->w + quaternion2->w;
				return quaternion;
			}

			static Quaternion^ operator /(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				Quaternion^ quaternion = gcnew Quaternion();
				float x = quaternion1->x;
				float y = quaternion1->y;
				float z = quaternion1->z;
				float w = quaternion1->w;
				float num14 = (((quaternion2->x * quaternion2->x) + (quaternion2->y * quaternion2->y)) + (quaternion2->z * quaternion2->z)) + (quaternion2->w * quaternion2->w);
				float num5 = 1.f / num14;
				float num4 = -quaternion2->x * num5;
				float num3 = -quaternion2->y * num5;
				float num2 = -quaternion2->z * num5;
				float num = quaternion2->w * num5;
				float num13 = (y * num2) - (z * num3);
				float num12 = (z * num4) - (x * num2);
				float num11 = (x * num3) - (y * num4);
				float num10 = ((x * num4) + (y * num3)) + (z * num2);
				quaternion->x = ((x * num) + (num4 * w)) + num13;
				quaternion->y = ((y * num) + (num3 * w)) + num12;
				quaternion->z = ((z * num) + (num2 * w)) + num11;
				quaternion->w = (w * num) - num10;
				return quaternion;
			}

			static bool operator ==(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				return ((((quaternion1->x == quaternion2->x) && (quaternion1->y == quaternion2->y)) && (quaternion1->z == quaternion2->z)) && (quaternion1->w == quaternion2->w));
			}

			static bool operator !=(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				if (((quaternion1->x == quaternion2->x) && (quaternion1->y == quaternion2->y)) && (quaternion1->z == quaternion2->z))
				{
					return (quaternion1->w != quaternion2->w);
				}
				return true;
			}

			static Quaternion^ operator -(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				Quaternion^ quaternion = gcnew Quaternion();
				quaternion->x = quaternion1->x - quaternion2->x;
				quaternion->y = quaternion1->y - quaternion2->y;
				quaternion->z = quaternion1->z - quaternion2->z;
				quaternion->w = quaternion1->w - quaternion2->w;
				return quaternion;
			}
			static Quaternion^ operator -(Quaternion^ quaternion)
			{
				return gcnew Quaternion(-quaternion->x, -quaternion->y, -quaternion->z, -quaternion->w);	
			}

			static Quaternion^ Conjugate(Quaternion^ value)
			{
				Quaternion^ quaternion;
				quaternion->x = -value->x;
				quaternion->y = -value->y;
				quaternion->z = -value->z;
				quaternion->w = value->w;
				return quaternion;
			}

			static Quaternion^ FromMatrix(glm::mat4 matrix)
			{
				return gcnew Quaternion(glm::quat(matrix));
			}

			static Quaternion^ EulerAngleDegrees(float x, float y, float z)
			{
				glm::vec3 EulerAngles(math_deg*x, math_deg *y, math_deg *z);
				glm::vec3 c = glm::cos(EulerAngles * 0.5f);
				glm::vec3 s = glm::sin(EulerAngles * 0.5f);
				float qw = c.x * c.y * c.z + s.x * s.y * s.z;
				float qx = s.x * c.y * c.z - c.x * s.y * s.z;
				float qy = c.x * s.y * c.z + s.x * c.y * s.z;
				float qz = c.x * c.y * s.z - s.x * s.y * c.z;
				Quaternion^ Q = gcnew Quaternion(qx,qy,qz,qw);
				return Q;
			}

			static Vector^ ToEulerAngles(Quaternion^ quat)
			{
				glm::vec3 euler = glm::eulerAngles(quat->glm());
				return gcnew Vector(euler.x, euler.y, euler.z);
			}

			Quaternion^ RotationAxis(Vector^ axis,float angle)
			{
				if (axis->lengthSquared() == 0.0f)
					return identity;

				Quaternion^ result = gcnew Quaternion();
				result->x = axis->x*sin(angle / 2);
				result->y = axis->y*sin(angle / 2);
				result->z = axis->z*sin(angle / 2);
				result->w = cos(angle / 2);

				return Normalize(result);
			}

			static Quaternion^ EulerAngleDegrees(Vector^ vec)
			{
				return Quaternion::EulerAngleDegrees(vec->x, vec->y, vec->z);
			}

			static Quaternion^ Divide(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				return quaternion1 / quaternion2;
			}

			static Quaternion^ EulerAngleRadians(Vector^ vec)
			{
				return gcnew Quaternion(glm::quat(glm::vec3(vec)));
			}

			static Quaternion^ RotationXYZ(Vector^ vector)
			{
				float sx = sin(vector->x / 2); 
				float sy = sin(vector->y / 2); 
				float sz = sin(vector->z / 2);
				float cx = cos(vector->x / 2); 
				float cy = cos(vector->y / 2); 
				float cz = cos(vector->z / 2);

				return gcnew Quaternion(cx*cy*cz + sx*sy*sz,
					sx*cy*cz - cx*sy*sz,
					cx*sy*cz + sx*cy*sz,
					cx*cy*sz - sx*sy*cz);
			}

			static Quaternion^ RotationZYX(Vector^ vector)
			{
				float sx = sin(vector->x / 2);
				float sy = sin(vector->y / 2);
				float sz = sin(vector->z / 2);
				float cx = cos(vector->x / 2);
				float cy = cos(vector->y / 2);
				float cz = cos(vector->z / 2);

				return gcnew Quaternion(cx*cy*cz - sx*sy*sz,
					sx*cy*cz + cx*sy*sz,
					cx*sy*cz - sx*cy*sz,
					cx*cy*sz + sx*sy*cz);
			}

			static Quaternion^ Concatenate(Quaternion^ value1, Quaternion^ value2)
			{
				Quaternion^ quaternion;
				float X = value2->x;
				float Y = value2->y;
				float Z = value2->z;
				float W = value2->w;
				float num4 = value1->x;
				float num3 = value1->y;
				float num2 = value1->z;
				float q2w = value1->w;
				float num12 = (Y * num2) - (Z * num3);
				float num11 = (Z * num4) - (X * num2);
				float num10 = (X * num3) - (Y * num4);
				float num9 = ((X * num4) + (Y * num3)) + (Z * num2);
				quaternion->x = ((X * q2w) + (num4 * W)) + num12;
				quaternion->y = ((Y * q2w) + (num3 * W)) + num11;
				quaternion->z = ((Z * q2w) + (num2 * W)) + num10;
				quaternion->w = (W * q2w) - num9;
				return quaternion;
			}

			static Quaternion^ Add(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				return quaternion1 + quaternion2;
			}

			static Quaternion^ Multiply(Quaternion^ quaternion1, Quaternion^ quaternion2)
			{
				return quaternion1*quaternion2;
			}
		};
	}
}

