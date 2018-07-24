#pragma once
#include "Vector.h"

namespace Antimatter {
	namespace Games {

		public ref class Shape2D
		{
		public:
			Shape2D()
			{

			}

			Shape2D(String^ Name, ...cli::array<Vector^>^ list)
			{
				name = Name;
				points = gcnew List<Vector^>(list->Length);
				for (int i = 0; i < list->Length; i++)
				{
					points->Add(list[i]);
				}
				ComputeMassProperties();
			}

			String^ name;
			List<Vector^>^ points = gcnew List<Vector^>();
			static List<Shape2D^>^ Shapes2d = gcnew List<Shape2D^>();
			float inertiaTensor;
			float area;
			bool debug = false;

			float CalculateArea()
			{
				float a = 0.f;
				int n = points->Count;
				for (int i = 0; i < n; i++)
				{
					int j = i + 1;
					if (j >= n)
						j = 0;

					a += (points[i]->x *points[j]->y) - (points[i]->y * points[j]->x);
				}
				area = abs(a);
				area *= 0.5f;
				return area;
			}

			void ComputeMassProperties()
			{
				float denom = 0.0f;
				float numer = 0.0f;
				float mass = 1.f;
				int Anum = points->Count;

				for (int j = Anum - 1, i = 0; i < Anum; j = i, i++)
				{
					Vector^ P0 = points[j];
					Vector^ P1 = points[i];

					float a = fabs(Vector::Cross2D(P0,P1));
					float b = (Vector::Dot2D(P1,P1) + Vector::Dot2D(P1,P0) + Vector::Dot2D(P0,P0));

					denom += a*b;
					numer += a;
		
				}
				float inertia = (mass / 6.0f) * (denom / numer);
				inertiaTensor = inertia;// 1.f; (numer / denom) / 6.0f;
				area = numer*0.5f;
				if (debug)
				{
					tool::show(this->name,"Denom: " + denom.ToString(), "Numer: "+ numer.ToString());
					tool::show("Inertia: "+inertiaTensor.ToString());
					tool::show("Area: "+area.ToString());
				}
			}
			
			//default box shape
			static Shape2D^ _box;
			static property Shape2D^ Box
			{
				Shape2D^ get()
				{
					if (_box == nullptr)
					{
						return _box = gcnew Shape2D("box", gcnew Vector(-1.f, -1.f, 0.f), gcnew Vector(1.f, -1.f, 0.f), gcnew Vector(1.f, 1.f, 0.f), gcnew Vector(-1.f, 1.f, 0.f));
					}
					else {
						return _box;
					}
				};
			}

			//default triangle shape
			static Shape2D^ _triangle;
			static property Shape2D^ Triangle
			{
				Shape2D^ get()
				{
					if (_triangle == nullptr)
					{
						return _triangle = gcnew Shape2D("triangle", gcnew Vector(1.f, -0.5f, 0.f), gcnew Vector(-1.f, -0.5f, 0.f), gcnew Vector(0.f, 1.f, 0.f));
					}
					else {
						return _triangle;
					}
				};
			}
		};
	}
}

