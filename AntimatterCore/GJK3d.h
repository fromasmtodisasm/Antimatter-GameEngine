#pragma once
#include "Vertex.h"
#include "Mesh.h"
#include "GJK2D.h"  //inclduing this for the evolve result enum.  Please build a gjk helper class instea
#include "Contact.h"
//https://stackoverflow.com/questions/31764305/im-implementing-the-expanding-polytope-algorithm-and-i-am-unsure-how-to-deduce
//http://allenchou.net/2013/12/game-physics-contact-generation-epa/

//https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=&f=4&t=288

//rk4 c++ physics engine
//https://github.com/Nightmask3/Physics-Framework/tree/master/PhysicsFramework

//closest distance
//https://www.medien.ifi.lmu.de/lehre/ss10/ps/Ausarbeitung_Beispiel.pdf
//MPR
//https://github.com/vaiorabbit/MPRTest/blob/master/MPRAlgorithm.rb
//https://github.com/erwincoumans/xenocollide/blob/master/XenoTestbed/Tests/TestXenoCollide/Collide.cpp


namespace Antimatter{
	namespace Games{

		//think we need to calculate barycentric coords of closestTriangle
		public ref class GJK3D
		{
		public:
			GJK3D()
			{
				direction = Vector::Zero;
			};

			ref class Triangle
			{
			public:
				Triangle()
				{
				}

				Triangle(Vector^ A, Vector^ B, Vector^ C)
				{
					a = A;
					b = B;
					c = C;
					colour = Colour::Purple;
				}

				Vector^ a;
				Vector^ b;
				Vector^ c;
				Colour^ colour;
				
				Vector^ Normal()
				{
					/*
					Vector^ u = b - a;
					Vector^ v = c - a;

					Vector^ n_ = gcnew Vector();
					n_->x = u->y*v->z - u->z * v->y;
					n_->y = u->z*v->x - u->x * v->z;
					n_->z = u->x*v->y - u->y * v->x;
					return -n_->Normalized();
					*/

					Vector^ v01 = b - a;
					Vector^ v02 = c - a;
					return -v01->cross(v02)->normalized();
				}

				Vector^ Centre()
				{
					return (a + b + c) / 3;
				}

				void Draw(Colour^ col)
				{
					Draw::Triangle(a, b, c, col);
				}

				void Debug(Colour^ col,Colour^ col2)
				{
					Vector^ c_ = Centre();
					Vector^ n = Normal();
					Colour^ simplexColour = col2;
					Colour^ f_ = col*0.8f;
					Draw(simplexColour);
					Draw::DebugLine(c_, c_ + n, f_,Colour::Yellow);
					Draw::DebugLine(c_, a, f_, simplexColour);
					Draw::DebugLine(c_, b, f_, simplexColour);
					Draw::DebugLine(c_, c, f_, simplexColour);
				}
			};

			ref class EPATriangle
			{
			public:
				EPATriangle()
				{

				};

				EPATriangle(SupportPoint^ A, SupportPoint^ B, SupportPoint^ C)
				{
					a = A;
					b = B;
					c = C;
					normal = Normal();
				};

				SupportPoint^ a;
				SupportPoint^ b;
				SupportPoint^ c;
				Vector^ normal;

				Vector^ Normal()
				{
					/*
					Vector^ u = b - a;
					Vector^ v = c - a;

					Vector^ n_ = gcnew Vector();
					n_->x = u->y*v->z - u->z * v->y;
					n_->y = u->z*v->x - u->x * v->z;
					n_->z = u->x*v->y - u->y * v->x;
					return -n_->Normalized();
					*/

					Vector^ v01 = b - a;
					Vector^ v02 = c - a;
					return v01->cross(v02)->normalized();
				}

				Vector^ Centre()
				{
					return (a + b + c) / 3;
				}

				void Draw(Colour^ col)
				{
					Draw::Triangle(a, b,c, col);
				}

				void Debug(List<SupportPoint^>^ simplex_,Colour^ col, Colour^ col2)
				{
					if (simplex_->Count < 4)
						return;
					Vector^ c_ = Centre();
					Vector^ n = normal;
					Colour^ simplexColour = col2;
					Colour^ f_ = col*0.8f;
					Draw(simplexColour);
					Draw::DebugLine(c_, c_ + n, f_, Colour::Yellow);
					Draw::DebugLine(c_, a, f_, simplexColour);
					Draw::DebugLine(c_, b, f_, simplexColour);
					Draw::DebugLine(c_, c, f_, simplexColour);
				}
			};

			ref class EPAEdge
			{
			public: 
				EPAEdge()
				{
				};

				SupportPoint^ a;
				SupportPoint^ b;
				EPAEdge(SupportPoint^ A, SupportPoint^ B)
				{
					a = A;
					b = B;
				}

				bool isEdge(SupportPoint^ A, SupportPoint^ B)
				{
					if (a->index == A->index && b->index == B->index)
						return true;
					return false;
				}

				bool isReverseEdge(SupportPoint^ A, SupportPoint^ B)
				{
					if (a->index == B->index && b->index == A->index)
						return true;
					return false;
				}

				bool isReverseEdge(EPAEdge^ edge)
				{
					if (a == edge->b && b == edge->a)
						return true;
					return false;
				}
			};

			List<SupportPoint^>^ simplex = gcnew List<SupportPoint^>();
			Vector^ direction;
			Mesh^ shapeA;
			Mesh^ shapeB;
			int maxIterations = 10;
			static int EPAITERARTIONS = 5;
			EPATriangle^ closestTriangle;
			Contact^ result;
			bool interseting;

			void barycentric(Vector^ p, Vector^ a, Vector^ b, Vector^ c, float% u, float% v, float% w)
			{
				// code from Crister Erickson's Real-Time Collision Detection
				Vector^ v0 = b - a;
				Vector^ v1 = c - a;
				Vector^ v2 = p - a;
				float d00 = v0->dot(v0);
				float d01 = v0->dot(v1);
				float d11 = v1->dot(v1);
				float d20 = v2->dot(v0);
				float d21 = v2->dot(v1);
				float denom = d00 * d11 - d01 * d01;
				v = (d11 * d20 - d01 * d21) / denom;
				w = (d00 * d21 - d01 * d20) / denom;
				u = 1.0f - v - w;
			}

			SupportPoint^ calculateSupport(Vector^ direction)
			{
				Vector^ oppositeDirection  = direction*-1;
				Vertex^ va  = shapeA->Support(direction);
				Vertex^ vb = shapeB->Support(oppositeDirection);
				SupportPoint^ sp = gcnew SupportPoint(va - vb, va, vb);
				return sp;
			}

			bool addSupport(Vector^ direction)
			{
				SupportPoint^ newVertex  = calculateSupport(direction);
				newVertex->index = simplex->Count;
				simplex->Add(newVertex);
				return Vector::Dot(direction, newVertex) >= 0.f;
			}

			bool Test(Mesh^ A, Mesh^ B)
			{
				// reset everything
				simplex->Clear();
				shapeA = A;
				shapeB = B;
				collision = false;
				closestTriangle = nullptr;
				result = nullptr;
				EvolveResult result = EvolveResult::StillEvolving;
				int iterations  = 0;
				while (iterations < maxIterations && result == EvolveResult::StillEvolving)
				{
					result = evolveSimplex();
					iterations++;
				}
				return result == EvolveResult::FoundIntersection;
			}

			Vector^ returnNormal;
			float depth;
			bool collision;
			Vector^ pointA;
			Vector^ pointB;
			Vector^ closestPoint;
			Vector^ seperation;
			List<Triangle^>^ polytope = gcnew List<Triangle^>();
			List<EPATriangle^>^ triangles = gcnew List<EPATriangle^>();
			List<EPAEdge^>^ edges = gcnew List<EPAEdge^>();

			void AddEdge(EPAEdge^ edge)
			{
				for (int i = 0; i < edges->Count; i++)
				{
					if (edge->isReverseEdge(edges[i]))
					{
						edges->Remove(edges[i]);
						return;
					}
				}
				edges->Add(edge);
			}

			EvolveResult evolveSimplex()
			{
				switch (simplex->Count)
				{
				case 0:
				{
					direction = shapeA->Position - shapeB->Position;
					break;
				}
				case 1:
				{
					// flip the direction
					direction *= -1.f;
					break;
				}
				case 2:
				{
					// line ab is the line formed by the first two vertices
					Vector^ ab = simplex[1] - simplex[0];
					// line a0 is the line from the first vertex to the origin
					Vector^ a0 = simplex[0] * -1.f;

					// use the triple-cross-product to calculate a direction perpendicular
					// to line ab in the direction of the origin
					Vector^ tmp = ab->cross(a0); //static Cross should be faster
					direction = Vector::TripleCross(tmp, ab, direction);
					break;
				}
				case 3:
				{
					Vector^ ac = simplex[2] - simplex[0];
					Vector^ ab = simplex[1] - simplex[0];
					direction = ac->cross(ab);

					// ensure it points toward the origin
					Vector^ a0 = simplex[0] * -1.f;
					if (direction->dot(a0) < 0.f)
						direction *= -1.f;
					break;
				}
				case 4:
				{
					// calculate the three edges of interest
					Vector^ da = simplex[3] - simplex[0];
					Vector^ db = simplex[3] - simplex[1];
					Vector^ dc = simplex[3] - simplex[2];

					// and the direction to the origin
					Vector^ d0 = -simplex[3];

					// check triangles a-b-d, b-c-d, and c-a-d
					Vector^ abdNorm = da->cross(db);
					Vector^ bcdNorm = db->cross(dc);
					Vector^ cadNorm = dc->cross(da);

					if (abdNorm->dot(d0) > 0.f) {
						// the origin is on the outside of triangle a-b-d
						// eliminate c!
						simplex->Remove(simplex[2]);
						direction = abdNorm;
					}
					else if (bcdNorm->dot(d0) > 0.f) {
						// the origin is on the outside of triangle bcd
						// eliminate a!
						simplex->Remove(simplex[0]);
						direction = bcdNorm;
					}
					else if (cadNorm->dot(d0) > 0.f) {
						// the origin is on the outside of triangle cad
						// eliminate b!
						simplex->Remove(simplex[1]);
						direction = cadNorm;
					}
					else {
						// the origin is inside all of the triangles!
						//	tool::debugBlue(EvolveResult::FoundIntersection);
						//if(Keyboard::defaultKeyboard->IsPressed(KeyButton::E))
						if (simplex->Count < 4)
						{
							tool::show("Handle simplex that is less than 4");
						}

						Vector^ v30 = simplex[0] - simplex[3];
						Vector^ v31 = simplex[1] - simplex[3];
						Vector^ v32 = simplex[2] - simplex[3];
						const float det = v30->dot(v31->cross(v32));
						if (det > 0.0f)
						{
							simplex->Reverse();
						}

						triangles->Clear();
						triangles->Add(gcnew EPATriangle(simplex[2], simplex[1], simplex[0]));
						triangles->Add(gcnew EPATriangle(simplex[3], simplex[2], simplex[0]));
						triangles->Add(gcnew EPATriangle(simplex[1], simplex[3], simplex[0]));
						triangles->Add(gcnew EPATriangle(simplex[2], simplex[3], simplex[1]));

						bool check_intersection=false;
						for (int i = 0; i < EPAITERARTIONS; i++)
						{
							check_intersection = RunEPA();
							if (check_intersection)
								break;
						}
						if (check_intersection)
							return EvolveResult::FoundIntersection;
						else
							return EvolveResult::NoIntersection;
					}
					break;
				}
				default:
					break;
				}

				if (addSupport(direction))
				{
					return EvolveResult::StillEvolving;
				}
				else
				{
					return EvolveResult::NoIntersection;
				}
			}

			bool RunEPA()
			{
				if (simplex->Count < 4)
				{
					tool::show("Handle simplex that is less than 4");
					return false;
				}
				else {
					//find the closest face of the polytope
					//closestTriangle = nullptr;
					EPATriangle^ current_tri = nullptr;
					float minkowski_dist = FLT_MAX;
					for each(EPATriangle^ t in triangles)
					{
						float dst = fabs(t->normal->dot(t->a));
						if (dst < minkowski_dist)
						{
							minkowski_dist = dst;
							current_tri = t;
						}
					}

					if (current_tri == nullptr)
						return false;

					//triangle normal should be pointing inwards based on other exmaples:  fix winding
					SupportPoint^ current_support = calculateSupport(current_tri->normal);
					if (current_tri->normal->length() <= 0.0001f)
					{
						tool::show("normal length of 0.f");
						//apply resting contact here
						return false;
					}

					float exit_threshold = 0.0001f;
					if (current_tri->normal->dot(current_support) - minkowski_dist < exit_threshold)
					{
						closestTriangle = current_tri;
						seperation = closestTriangle->normal*minkowski_dist;
						return GetContactInfo(minkowski_dist, current_tri);
					}
					edges->Clear();

					for (int i = 0; i<triangles->Count;i++)
					{
						EPATriangle^ t = triangles[i];
						bool seen = t->normal->dot(current_support - t->a) > 0.f;
						if (seen)
						{
							EPAEdge^ edge1 = gcnew EPAEdge(t->a, t->b);
							EPAEdge^ edge2 = gcnew EPAEdge(t->b, t->c);
							EPAEdge^ edge3 = gcnew EPAEdge(t->c, t->a);
							AddEdge(edge1);
							AddEdge(edge2);
							AddEdge(edge3);
							triangles->Remove(t);
						}
					}

					simplex->Add(current_support);
					current_support->index = simplex->Count-1;
					for each(EPAEdge^ edge in edges)
					{
						EPATriangle^ t = gcnew EPATriangle(current_support, edge->a, edge->b);
						triangles->Add(t);
					}
					edges->Clear();
				}
				return false;
			}

			

			bool GetContactInfo(float currentDist_, EPATriangle^ tri)
			{
				float u;
				float v;
				float w;
				barycentric(tri->normal*currentDist_,tri->a,tri->b,tri->c,u,v,w);


				if (float::IsNaN(u) || float::IsNaN(v) || float::IsNaN(w))
					return false;

				if (fabs(u) > 1.f || fabs(v) > 1.f || fabs(w) > 1.f)
					return false;

				result = gcnew Contact(shapeA,shapeB);

				result->point1 = (tri->a->vertexA * u) +
					(tri->b->vertexA* v) +
					(tri->c->vertexA * w);
				//two methods for computing the next point
				//result->point2 = result->point1 + result->normal*depth;
				result->point2 = (tri->a->vertexB * u) +(tri->b->vertexB* v) +(tri->c->vertexB * w);

				result->localA = glm::inverse(shapeA->gxtransform->staticMatrix)*result->point1;
				result->localB = glm::inverse(shapeB->gxtransform->staticMatrix)*result->point2;

				result->r1 = result->point1 - shapeA->Position;
				result->r2 = result->point2 - shapeB->Position;
				result->depth = currentDist_;
				result->normal = -tri->normal->normalized();
				collision = true;
				return true;
			}

			void DebugSimplex()
			{
				for(int i=0;i<triangles->Count;i++)
				{
					EPATriangle^ t = triangles[i];
					Colour^ spc = Colour::Blue*0.8f;
					Colour^ scol = Colour::Violet;
					if(t != closestTriangle)
						t->Debug(simplex, spc, scol);
					t->Centre()->DrawAs(i.ToString(), spc, 0.5f);
				}

				if (closestTriangle == nullptr)
					return;

				closestTriangle->Debug(simplex, Colour::Green, Colour::Red);
				Vector::Zero->DrawAs(". Origin", Colour::White, 0.2f);

				if (result != nullptr)
				{
					Vector^ projection = result->normal*-result->depth;
					Draw::DebugLine(Vector::Zero, projection, Colour::Vermillion);
					projection->DrawAsPointRaw(Colour::New, 0.05f);
					Draw::DebugLine(result->point1, result->point1 + result->normal, Colour::LightBlue);
					//result->worldPointA->DrawAsPointRaw(Colour::New, 0.05f);
					//result->worldPointB->DrawAsPointRaw(Colour::New, 0.05f);
				}
			}

			void DrawSimplex(Colour^ col)
			{
				Vector::Zero->DrawAs("Origin", Colour::DarkGreen);
				Vector::Zero->DrawAsPointRaw(Colour::New, 0.6f);
				//Draw::Triangle(simplex[0],simplex[1], simplex[2],col);
				//Draw::Triangle(simplex[3], simplex[1], simplex[2], col);
				//Draw::Triangle(simplex[3], simplex[0], simplex[1], col);
				for (int i = 0; i < simplex->Count; i++)
				{
					String^ text = "A";
					if (i == 1)
						text = "B";
					if (i == 2)
						text = "C";
					if (i == 3)
						text = "D";
					if (i == 4)
						text = "E";
					if (i == 5)
						text = "F";
					if (i == 6)
						text = "G";
					simplex[i]->DrawAs(text, Colour::White);
				}
			}
		};
	}
}
