#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Vector.h"

namespace Antimatter {
	namespace Games {

		public ref class Mat2x2
		{
		public:
			float m00, m01;
			float m10, m11;

			Mat2x2(Vector^ axisx, Vector^ axisy)
			{
				m00 = axisx->x;
				m01 = axisx->y;
				m10 = axisy->x;
				m11 = axisy->y;
			}

			Mat2x2()
			{
				m00 = 0.f;
				m01 = 1.f;
				m10 = 1.f;
				m11 = 0.f;
			}

			Mat2x2(float radians)
			{
				Set(radians);
			}

			void Set(float radians)
			{
				float c = std::cos(radians);
				float s = std::sin(radians);

				m00 = c;
				m01 = -s;
				m10 = s;
				m11 = c;
			}

			static Mat2x2^ operator*(Mat2x2^ t1, Mat2x2^ t2)
			{
				Mat2x2^ t = gcnew Mat2x2();
				t->m00 = (t1->m00 * t2->m00) + (t1->m01*t2->m10);
				t->m01 = (t1->m00*t2->m10) + (t1->m01*t2->m11);
				t->m10 = (t1->m10*t2->m00) + (t1->m11*t2->m10);
				t->m11 = (t1->m10*t2->m01) + (t1->m11*t2->m11);

				return t;
			}

			void Show()
			{
				tool::show("m00: " + m00.ToString());
				tool::show("m01: " + m01.ToString());
				tool::show("m10: " + m10.ToString());
				tool::show("m11: " + m11.ToString());
			}

			static void Show(glm::mat2 mat)
			{
				tool::show("m00: " + mat[0][0].ToString(),
							"m01: " + mat[0][1].ToString(),
							"m10: " + mat[1][0].ToString(),
							"m11: " + mat[1][1].ToString());
			}

			static Mat2x2^ operator*(Mat2x2^ t1, float scaler)
			{
				Mat2x2^ t = gcnew Mat2x2();
				t->m00 = t1->m00 * scaler;
				t->m01 = t1->m01 * scaler;
				t->m10 = t1->m10 * scaler;
				t->m11 = t1->m11 * scaler;
				return t;
			}

			glm::mat2 glm()
			{
				glm::mat2 mat = glm::mat2(1.f);
				mat[0][0] = m00;
				mat[0][1] = m01;
				mat[1][0] = m10;
				mat[1][1] = m11;
				return mat;
			}

			float Determinate()
			{
				float d = (m00*m11) - (m01*m10);
				if (d == 0.f)
					tool::show("Warning!  Determinate is 0!");
				return d;
			}

			Vector^ operator*(Vector^ rhs)
			{
				return gcnew Vector(m00 * rhs->x + m01 * rhs->y, m10*rhs->x + m11 * rhs->y);
			}

			property Vector^ Xcolumn
			{
				Vector^ get()
				{
					return gcnew Vector(m00, m01);
				}

				void set(Vector^ v)
				{
					m00 = v->x;
					m01 = v->y;
				}
			}

			property Vector^ Ycolumn
			{
				Vector^ get()
				{
					return gcnew Vector(m10, m11);
				}

				void set(Vector^ v)
				{
					m10 = v->x;
					m11 = v->y;
				}
			}
		};

		public ref class Transform2D : Mat2x2
		{
		public:

			Transform2D() : Mat2x2()
			{
				position = Vector::Zero;
				scale = gcnew Vector(1.f,1.f,0.f,0.f);
			};

			Vector^ Rotate(Vector^ rhs)
			{
				return gcnew Vector(m00 * rhs->x + m01 * rhs->y, m10*rhs->x + m11 * rhs->y);
			}

			Vector^ Translate(Vector^ v)
			{
				return position + v;
			}

			Vector^ Scale(Vector^ v)
			{
				return gcnew Vector(v->x*scale->x, v->y*scale->y);
			}

			Vector^ Transform(Vector^ rhs)
			{
				//no scaling applied yet
				Vector^ v = Rotate(rhs);
				v = Translate(v);
				v = Scale(v);
				return v;
			}

			void UpdateMatrix()
			{
				float c = std::cos(rotation);
				float s = std::sin(rotation);

				m00 = c;
				m01 = -s;
				m10 = s;
				m11 = c;
			}

			Vector^ position;
			Vector^ scale;
			float rotation;
			float sort;
		};
	}
}

