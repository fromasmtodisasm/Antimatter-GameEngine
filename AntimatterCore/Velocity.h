#pragma once
#include "Vector.h"

namespace Antimatter {
	namespace Games {

		public ref class Velocity
		{
		public:


			Vector^ linear;
			Vector^ angular;
			static Velocity^ Zero = gcnew Velocity();

			Velocity()
			{
				linear = Vector::Zero;
				angular = Vector::Zero;
			}

			Velocity(Vector^ Linear, Vector^ Angular)
			{
				linear = Linear;
				angular = Angular;
			}

			Velocity(const Velocity^% velocity)
			{
				linear = velocity->linear;
				angular = velocity->angular;
			}

			Vector^ AtPoint(Vector^ point)
			{
				return linear + angular->cross(point);
			}

			String^ ToString() override
			{
				return String::Concat("\nLinear:", linear, "\nAngular:", angular);
			}

			void Show()
			{
				tool::show(this);
			}

			void Show(String^ text)
			{
				tool::show(text,this);
			}

			Velocity^ Negated()
			{
				return gcnew Velocity(-linear,-angular);
			}

			static Velocity^ operator -(Velocity^ v) {
				return v->Negated();
			}

			static Velocity^ operator /(const Velocity^ a, const Velocity^ b)
			{
				return Velocity::Divide(a, b);
			}

			static Velocity^ operator /(const Velocity^ a, float b)
			{
				return Velocity::Divide(a, b);
			}

			static Velocity^ Divide(const Velocity^ value1, const Velocity^ value2)
			{
				return gcnew Velocity(value1->linear / value2->linear, value1->linear / value2->angular);
			}

			static Velocity^ Divide(const Velocity^ value1, float value2)
			{
				
				float factor = 1.f / value2;
				return gcnew Velocity(value1->linear*factor,value1->angular*factor);
			}

			static Velocity^ operator*(const Velocity^ vec1, const Velocity^ vec2)
			{
				return gcnew Velocity(vec1->linear*vec2->linear, vec1->angular*vec2->angular);
			}

			static Velocity^ operator+(const Velocity^ vec1, const Velocity^ vec2)
			{
				Velocity^ v = gcnew Velocity();
				v->linear = vec1->linear + vec2->linear;
				v->angular = vec1->angular + vec2->angular;
				return v;
			}

			static Velocity^ operator-(const Velocity^ vec1, const Velocity^ vec2)
			{
				Velocity^ v = gcnew Velocity();
				v->linear = vec1->linear - vec2->linear;
				v->angular = vec1->angular - vec2->angular;
				return v;
			}

			static Velocity^ operator*(Velocity^ vec1, float mult)
			{
				return gcnew Velocity(vec1->linear*mult,vec1->angular*mult);
			}

			static Velocity^ operator*(float mult, Velocity^ vec1)
			{
				return vec1*mult;
			}
		};
	}
}
