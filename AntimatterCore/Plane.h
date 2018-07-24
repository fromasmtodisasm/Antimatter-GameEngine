#pragma once
#include "Vector.h"
//#include "Renderer.h"
//http://www.cplusplus.com/forum/general/74720/
//https://books.google.co.uk/books?id=p4bKlsSHj3QC&pg=PA283&lpg=PA283&dq=normalize+a+plane&source=bl&ots=H9NYqrtI-e&sig=eytoEARDGpz1qn9QzWI3lVE3aWc&hl=en&sa=X&ved=0ahUKEwjoldCV3MLVAhWhCcAKHT5sAQkQ6AEIkQEwEg#v=onepage&q=normalize%20a%20plane&f=false

namespace Antimatter{
	namespace Games {
		/*
		Plane.cpp
		Written by Matthew Fisher

		A standard 3D plane (space plane.)  Essentially just the surface defined by a*x + b*y + c*z + d = 0
		See Plane.h for a description of these functions.
		*/
		public ref class Plane
		{
		public:

			Plane::Plane()
			{

			}
			/*
			Plane::Plane(const Plane &P)
			{
				a = P.a;
				b = P.b;
				c = P.c;
				d = P.d;
			}
			*/
			float x;
			float y;
			float z;
			float distance;
			property Vector^ normal
			{
				Vector^ get()
				{
					return gcnew Vector(x, y, z);
				}
				void set(Vector^ v)
				{
					x = v->x;
					y = v->y;
					z = v->z;
				}
			};

			Plane::Plane(float X, float Y, float Z, float Distance)
			{
				x = X;
				y = Y;
				z = Z;
				distance = Distance;
			}

			Plane::Plane(Vector^ NormalizedNormal, float Distance)
			{
				x = NormalizedNormal->x;
				y = NormalizedNormal->y;
				z = NormalizedNormal->z;
				distance = Distance;
			}

			Plane^ Plane::ConstructFromPointNormal(Vector^ Pt, Vector^ Normal)
			{
				Plane^ Result = gcnew Plane();
				Vector^ NormalizedNormal = Normal->normalized();
				Result->x = NormalizedNormal->x;
				Result->y = NormalizedNormal->y;
				Result->z = NormalizedNormal->z;
				Result->distance = -Vector::Dot(Pt, NormalizedNormal);
				return Result;
			}

			Plane^ Plane::ConstructFromPointVectors(Vector^ Pt, Vector^ V1, Vector^ V2)
			{
				Vector^ Normal = Vector::Cross(V1, V2);
				return ConstructFromPointNormal(Pt, Normal);
			}

			Plane^ Plane::Normalize()
			{
				Plane^ Result = gcnew Plane();
				float Distance = sqrtf(x * x + y * y + z * z);
				Result->x = x / Distance;
				Result->y = y / Distance;
				Result->z = z / Distance;
				Result->distance = distance / Distance;
				return Result;
			}

			static Plane^ Plane::FromPoints(Vector^ V0, Vector^ V1, Vector^ V2)
			{
				Vector^ Normal = Vector::Cross(V1 - V0, V2 - V0)->normalized();
				Plane^ p = gcnew Plane();
				return p->ConstructFromPointNormal(V0, Normal);
			}

			Vector^ Plane::IntersectLine(Vector^ V1, Vector^ V2)
			{
				Vector^ Diff = V1 - V2;
				float Denominator = x * Diff->x + y * Diff->y + z * Diff->z;
				if (Denominator == 0.0f)
				{
					return (V1 + V2) * 0.5f;
				}
				float u = (x * V1->x + y * V1->y + z * V1->z + distance) / Denominator;

				return (V1 + u * (V2 - V1));
			}

			Vector^ Plane::IntersectLine(Vector^ V1, Vector^ V2, bool &Hit)
			{
				Hit = true;
				Vector^ Diff = V2 - V1;
				float denominator = x * Diff->x + y * Diff->y + z * Diff->z;
				if (denominator == 0) { Hit = false; return V1; }
				float u = (x * V1->x + y * V1->y + z * V1->z + distance) / denominator;

				return (V1 + u * (V2 - V1));
			}

			float Plane::IntersectLineRatio(Vector^ V1, Vector^ V2)
			{
				Vector^ Diff = V2 - V1;
				float Denominator = x * Diff->x + y * Diff->y + z * Diff->z;
				if (Denominator == 0.0f)
				{
					return 0.0f;
				}
				return (x * V1->x + y * V1->y + z * V1->z + distance) / -Denominator;
			}

			float Plane::SignedDistance(Vector^ Pt)
			{
				return (x * Pt->x + y * Pt->y + z * Pt->z + distance);
			}

			float Plane::UnsignedDistance(Vector^ Pt)
			{
				return Math::Abs(x * Pt->x + y * Pt->y + z * Pt->z + distance);
			}

			Vector^ Plane::ClosestPoint(Vector^ Point)
			{
				return (Point - normal * SignedDistance(Point));
			}

			float Plane::Dot(Plane^ P, glm::vec4 V)
			{
				return P->x * V.x + P->y * V.y + P->z * V.z + P->distance * V.w;
			}

			float Plane::DotCoord(Plane^ P, Vector^ V)
			{
				return P->x * V->x + P->y * V->y + P->z * V->z + P->distance;
			}

			float Plane::DotNormal(Plane^ P, Vector^ V)
			{
				return P->x * V->x + P->y * V->y + P->z * V->z;
			}
		};
	}
}