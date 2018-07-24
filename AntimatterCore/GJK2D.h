#pragma once
#include "Vector.h"
#include "SubScene.h"
#include "Collider2d.h"
#include "Contact2d.h"

//https://github.com/FuzzyWuzzie/headbutt/blob/master/src/headbutt/Headbutt2D.hx
//https://gamedevelopment.tutsplus.com/tutorials/how-to-create-a-custom-2d-physics-engine-oriented-rigid-bodies--gamedev-8032

//physics 2d source code
//https://github.com/tutsplus/ImpulseEngine

namespace Antimatter {
	namespace Games {

		public enum class  EvolveResult { NoIntersection, FoundIntersection, StillEvolving };
		public enum class PolygonWinding{ CounterClockwise,Clockwise};

		public ref class GJK2D : public SubScene
		{
		public:

			ref class SimplexEdge
			{
			public:
				SimplexEdge()
				{
				}

				SimplexEdge(float d, Vector^ n, int i)
				{
					this->distance = d;
					this->normal = n;
					this->index = i;
				}

				float distance;
				Vector^ normal;
				int index;
			};

			GJK2D(Physics2d^ p1,Physics2d^ p2)
			{
				shapeA = p1;
				shapeB = p2;
			}

			List<SupportPoint^>^ vertices = gcnew List<SupportPoint^>();
			Vector^ direction;
			Vector^ initialDirection;
			Physics2d^ shapeA;
			Physics2d^ shapeB;
			SupportPoint^ closestSimplexPointB;
			SupportPoint^ closestSimplexPointA;
			Vector^ contactA;
			Vector^ contactB;
			EvolveResult^ evolveResult;
			int maxIterations = 10;

			//watch this.  check support function
			SupportPoint^ calculateSupport(Vector^ d)
			{
				Vertex^ vertexA = shapeA->collider->GetSupportPoint(d);
				Vertex^ vertexB = shapeB->collider->GetSupportPoint(d*-1.f);
				return gcnew SupportPoint(vertexA - vertexB,vertexA,vertexB);
			}

			bool addSupport(Vector^ d)
			{
				SupportPoint^ newVertex = calculateSupport(d);
				vertices->Add(newVertex);
				return d->dot2d(newVertex) >= 0.f;
			}

			Vector^ tripleProduct(Vector^ a, Vector^ b, Vector^ c)
			{
				Vector^ A = gcnew Vector(a->x, a->y, 0);
				Vector^ B = gcnew Vector(b->x, b->y, 0);
				Vector^ C = gcnew Vector(c->x, c->y, 0);

				Vector^ first = Vector::Cross(A, B);
				Vector^ second = Vector::Cross(first, C);

				return gcnew Vector(second->x, second->y, 0.f);
			}

			EvolveResult evolveSimplex()
			{
				float e_limit = FLT_EPSILON;
				switch (vertices->Count)
				{
					case 0: {
						direction = shapeB->transform->position - shapeA->transform->position;
						break;
					}
					case 1: {
						// flip the direction
						direction *= -1.f;
						break;
					}
					case 2: {
						// line ab is the line formed by the first two vertices
						Vector^ ab = vertices[1] - vertices[0];
						// line a0 is the line from the first vertex to the origin
						Vector^ a0 = vertices[0] * -1.f;

						// use the triple-cross-product to calculate a direction perpendicular
						// to line ab in the direction of the origin
						direction = tripleProduct(ab, a0, ab);
						
						if (direction->lengthSquared2D() < e_limit*e_limit)
							vertices[0]->DisplayAs("origin on line segemnt");
						break;
					}
					case 3: {
						// calculate if the simplex contains the origin
						Vector^ a0 = vertices[2] * -1; //v2 to the origin
						Vector^ ab = vertices[1] - vertices[2]; //v2 to v1
						Vector^ ac = vertices[0] - vertices[2]; //v2 to v0

						Vector^ abNorm = tripleProduct(ac, ab, ab);
						Vector^ acNorm = tripleProduct(ab, ac, ac);

						if (abNorm->lengthSquared2D() < e_limit*e_limit)
							vertices[0]->DisplayAs("origin on line segemnt");
						if (acNorm->lengthSquared2D() < e_limit*e_limit)
							vertices[0]->DisplayAs("origin on line segemnt");

						if (abNorm->dot(a0) > 0.f) {
							// the origin is outside line bc
							// get rid of c and add a new support in the direction of bcNorm
							vertices->Remove(vertices[0]);
							direction = abNorm;
						}
						else if (acNorm->dot(a0) > 0.f) {
							// the origin is outside line ca
							// get rid of b and add a new support in the direction of caNorm
							vertices->Remove(vertices[1]);
							direction = acNorm;
						}
						else {
							// the origin is inside both ab and ac,
							// so it must be inside the triangle!
							//evolveResult->FoundIntersection = true;
							return EvolveResult::FoundIntersection;
						}
						break; 
					}
					default:
						break;
				}

				if (addSupport(direction))
					return EvolveResult::StillEvolving;
				else
					return EvolveResult::NoIntersection;
			}

			bool test(Physics2d^ shapeA, Physics2d^ shapeB)
			{
				// reset everything
				this->vertices->Clear();
				this->shapeA = shapeA;
				this->shapeB = shapeB;

				// do the actual test
				EvolveResult result = EvolveResult::StillEvolving;
				int iterations = 0;
				while (iterations < maxIterations && result == EvolveResult::StillEvolving)
				{
					result = evolveSimplex();
					iterations++;
				}
				return result == EvolveResult::FoundIntersection;
			}

			SimplexEdge^ findClosestEdge(PolygonWinding winding)
			{
				float closestDistance = FLT_MAX;
				Vector^ closestNormal = gcnew Vector();
				int closestIndex = 0;
				Vector^ line = gcnew Vector();

				for (int i = 0; i < vertices->Count; i++)
				{
					int j = i + 1;
					if (j >= vertices->Count)
						j = 0;

					line = vertices[i] - vertices[j];  //check the subtraction direction here
					Vector^ norm;

					switch (winding)
					{
					case PolygonWinding::Clockwise:
						norm = gcnew Vector(line->y, -line->x); // left
						break;
					case PolygonWinding::CounterClockwise:
						norm = gcnew Vector(-line->y, line->x);  // right
						break;
					}

					norm = norm->normalize2D();
					// calculate how far away the edge is from the origin
					float dist = norm->dot(vertices[i]);
					if (dist < closestDistance)
					{
						closestDistance = dist;
						closestNormal = norm;
						closestIndex = j;
						closestSimplexPointA = vertices[i];
						closestSimplexPointB = vertices[j];
					}
				}
				return gcnew SimplexEdge(closestDistance, closestNormal, closestIndex);
			}

			SimplexEdge^ Intersect(Physics2d^ shapeA, Physics2d^ shapeB)
			{
				// first, calculate the base simplex
				if (!test(shapeA, shapeB))
				{
					// if we're not intersecting, return null
					return nullptr;
				}
				// calculate the winding of the existing simplex
				float e0 = (vertices[1]->x - vertices[0]->x) * (vertices[1]->y + vertices[0]->y);
				float e1 = (vertices[2]->x - vertices[1]->x) * (vertices[2]->y + vertices[1]->y);
				float e2 = (vertices[0]->x - vertices[2]->x) * (vertices[0]->y + vertices[2]->y);
				PolygonWinding winding;
				if (e0 + e1 + e2 >= 0)
					winding = PolygonWinding::Clockwise;
				else
					winding = PolygonWinding::CounterClockwise;

				SimplexEdge^ edge = gcnew SimplexEdge();
				for (int i = 0; i < 32; i++)
				{
					edge = findClosestEdge(winding);
					SupportPoint^ support = calculateSupport(edge->normal);
					float distance = support->dot(edge->normal);

					if (abs(distance - edge->distance) <= 0.000001f)
					{
						return edge;
					}
					else {
						vertices->Insert(edge->index, support);
					}
				}
				return edge;
			}

			void FindContactPoints()
			{
				//closest edge.  We can catch this eariler when computing the closest edge
				Vector^ L = closestSimplexPointB - closestSimplexPointA;
				if (L->length() < 0.00001f)
				{
					contactA = closestSimplexPointA->vertexA;
					contactB = closestSimplexPointA->vertexB;
					return;
				}

				float delta2 = (-L->dot(closestSimplexPointA) / (L->dot(L)));
				float delta1 = 1.f - delta2;
				if (delta1 < 0.f)
				{
					contactA = closestSimplexPointB->vertexA;
					contactB = closestSimplexPointB->vertexB;
					return;
				}
				else if (delta2 < 0.f)
				{
					contactA = closestSimplexPointA->vertexA;
					contactB = closestSimplexPointA->vertexB;
					return;
				}

				contactA = closestSimplexPointA->vertexA*delta1 + closestSimplexPointB->vertexA*delta2;
				contactB = closestSimplexPointA->vertexB*delta1 + closestSimplexPointB->vertexB*delta2;
			}

			Contact2d^ FindContact(Physics2d^ p1,Physics2d^ p2)
			{
				SimplexEdge^ i = Intersect(p1,p2);
				if (i == nullptr)
					return nullptr;

				//no need to compute the closest points
				Contact2d^ cp = gcnew Contact2d();
				cp->a = p1;
				cp->b = p2;
				cp->normal = i->normal;
				cp->depth = i->distance;
				cp->indexA = closestSimplexPointA->vertexA->index;
				cp->indexB = closestSimplexPointA->vertexB->index;
				return cp;
			}

			void Load() override
			{
			}
			
			void Update() override
			{
				
			}
		};
	}
}

