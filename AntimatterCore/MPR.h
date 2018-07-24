#pragma once
#include "Mesh.h"
#include "Contact.h"
//https://pybullet.org/Bullet/phpBB3/viewtopic.php?f=6&t=1964&start=15

//closest point using gjk
//http://www.dyn4j.org/2010/04/gjk-distance-closest-points/

//MPR c#
//https://gamedev.stackexchange.com/questions/44709/how-can-i-test-if-my-rotated-rectangle-intersects-a-corner/45692#45692

//collision detection test
//https://github.com/j-rivero/libccd/blob/master/src/mpr.c

//point based gpu collision
//https://graphics.ethz.ch/Downloads/Publications/Papers/2004/Kei04/Kei04.pdf
//volumetric collision detection
//http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.13.2297&rep=rep1&type=pdf
//https://forum.unity.com/threads/gpu-physics.438244/
//https://github.com/jknightdoeswork/gpu-physics-unity/blob/master/Assets/Physics/InstancedIndirectSurfaceShader.shader

namespace Antimatter {
	namespace Games {

		public ref class MPR
		{
		public:
			MPR()
			{

			};

			Vector^ globalPoint1;
			Vector^ globalPoint2;

			MPR(Mesh^ a, Mesh^ b)
			{
				A = a;
				B = b;
			}

			void Swap(Vector^% a, Vector^% b)
			{
				Vector^ tmp = a;
				a = b;
				b = tmp;
			}

			static Vertex^ TransformSupportVert(Mesh^ mesh, Vector^ direction)
			{
				
				Vector^ local_n = mesh->Rotation->inverse()*direction->W(0.f);
				Vertex^ local_v = GetSupportVert(mesh, local_n);
				return gcnew Vertex(mesh*local_v,local_v->index);
			}

			static Vertex^ GetSupportVert(Mesh^ mesh, Vector^ direction)
			{
				int n_points = mesh->shape->points->Count;
				if (n_points == 0)
					return nullptr;

				int best = 0;
				float best_dot = Vector::Dot(mesh->Scale*mesh->shape->points[0], direction);

				for (int i = 1; i < n_points; i++)
				{
					float d = Vector::Dot(mesh->Scale*mesh->shape->points[i], direction);
					if (d > best_dot)
					{
						best = i;
						best_dot = d;
					}
				}

				return mesh->shape->points[best];
			}

			static Vertex^ GetSupportVert(List<Vertex^>^ points,Vector^ direction)
			{
				unsigned int n_points = points->Count;
				if (n_points == 0)
					return nullptr;

				unsigned int best = 0;
				float best_dot = Vector::Dot(points[0], direction);

				for (unsigned int i = 1; i < n_points; i++)
				{
					float d = Vector::Dot(points[i], direction);
					if (d > best_dot)
					{
						best = i;
						best_dot = d;
					}
				}

				return points[best];
			}

			static Vertex^ GetLocalSupportVert(List<Vertex^>^ points, Vector^ direction,Mesh^ mesh,Vector^ t)
			{
				unsigned int n_points = points->Count;
				if (n_points == 0)
					return nullptr;

				unsigned int best = 0;
				Vector^ d = mesh->Rotation->inverse()*direction;
				float best_dot = Vector::Dot(points[0], direction);

				for (unsigned int i = 1; i < n_points; i++)
				{
					float d = Vector::Dot(points[i], direction);
					if (d > best_dot)
					{
						best = i;
						best_dot = d;
					}
				}

				return gcnew Vertex(mesh->Rotation*points[best] + t,best);
			}

			Contact^ Test(Mesh^ p1, Mesh^ p2)
			{
				Contact^ c = gcnew Contact(p1,p2);
				bool collided = CollideAndFindPoint(p1, p2, c->normal, c->point1, c->point2);

				if (!collided)
					return nullptr;
				return c;
			}

			bool CollideAndFindPoint(Mesh^ p1,   Mesh^ p2, Vector^% returnNormal, Vector^% point1, Vector^% point2)
			{
				int supportCount = 0;

				float kCollideEpsilon = 1e-3f;
				int callCount = 0;

				callCount++;
				A = p1;
				B = p2;
				//List<Vertex^>^ m1 = A->GetTransformedPoints();
				//List<Vertex^>^ m2 = B->GetTransformedPoints();
				
				//List<Vertex^>^ m1 = A->GetScaledPoints();
				//List<Vertex^>^ m2 = B->GetScaledPoints();
				// v0 = center of Minkowski sum
				Vector^ t1 = Vector::Zero; //eventually use velocity here
				Vector^ t2 = Vector::Zero;
				//t1 = A->Position - (A->Rotation*A->Position); //todo:  use actual R*COMS
				//t2 = B->Position - (B->Rotation*B->Position);
				Vector^ v01 = A->Rotation*A->Position + t1; //or use local centre of points and convert to worldspace
				Vector^ v02 = B->Rotation*B->Position + t2;  //do we really need rotation here?
				Vector^ v0 =  v02 - v01;

				// Avoid case where centers overlap -- any direction is fine in this case
				if (v0->length() < 0.001f) //use correct epislon.  See if Length can be sqaured.
					v0 = gcnew Vector(0.0f, 0.9f, 0.f); 

				// v1 = support in direction of origin
				Vector^ n = -v0; //point to origin
				supportCount++;
				Vector^ v11 = TransformSupportVert(A, -n) + t1;
				Vector^ v12 = TransformSupportVert(B, n) + t2;
				//Vector^ v11 = GetLocalSupportVert(m1, -n, A, t1); //+t1
				//Vector^ v12 = GetLocalSupportVert(m1, n, B, t2); //+t2
				Vector^ v1 = v12 - v11;
				//DebugPushVector(v0, v0 + n, 4);
				//DebugPushPoint(v1, 4);
				if (v1->dot(n) <= 0.f)
				{
					if (returnNormal)
						returnNormal = n;
					return false;
				}

				// v2 - support perpendicular to v1,v0
				n = v1 % v0;
				if (n->length() < 0.0001f) //check for zero vector
				{
					n = v1 - v0;
					n = n->normalized();
					if (returnNormal)  //these checks are probably unecessary
						returnNormal = n;
					if (point1)
						point1 = v11;
					if (point2)
						point2 = v12;
					return true;
				}
				supportCount++;
				Vector^ v21 = TransformSupportVert(A, -n) + t1;
				Vector^ v22 = TransformSupportVert(B, n) + t2;
				//Vector^ v21 = GetLocalSupportVert(m1, -n, A, t1); //+t;
				//Vector^ v22 = GetLocalSupportVert(m2, n, B, t2); //+t2
				Vector^ v2 = v22 - v21;

				if ((v2 ^ n) <= 0.f)
				{
					if (returnNormal)
						returnNormal = n;
					return false;
				}

				// Determine whether origin is on + or - side of plane (v1,v0,v2)
				n = (v1 - v0) % (v2 - v0);
				float dist = n ^ v0;
				
				if (n->length() < 0.0001f)
					tool::show("origin is on the plane?");
				//ASSERT(!n.IsZero3());

				// If the origin is on the - side of the plane, reverse the direction of the plane
				if (dist > 0.f)
				{
					Swap(v1, v2);
					Swap(v11, v21);
					Swap(v12, v22);
					n = -n;
				}
				
				///
				// Phase One: Identify a portal
				int p1_limit = 0;
				while (p1_limit<50)  //hack to cap iteration.  Some touching/sliding contacts were causing the loop to get stuck
				{
					p1_limit++;
					// Obtain the support point in a direction perpendicular to the existing plane
					// Note: This point is guaranteed to lie off the plane

					supportCount++;
					Vector^ v31 = TransformSupportVert(A, -n) + t1;
					Vector^ v32 = TransformSupportVert(B, n) + t2;
					Vector^ v3 = v32 - v31;

					if ((v3 ^ n) <= 0.00001f)
					{
						if (returnNormal)
							returnNormal = n;
						return false;
					}

					// If origin is outside (v1,v0,v3), then eliminate v2 and loop
					if (((v1 % v3)^v0) <= 0.0f)  //modified to <=
					{
						v2 = v3;
						v21 = v31;
						v22 = v32;
						n = (v1 - v0) % (v3 - v0);
						continue;
					}

					// If origin is outside (v3,v0,v2), then eliminate v1 and loop
					if (((v3 % v2) ^ v0) <= 0.0f)  //also modified from < 
					{
						v1 = v3;
						v11 = v31;
						v12 = v32;
						n = (v3 - v0) % (v2 - v0);
						continue;
					}

					bool hit = false;

					// Phase Two: Refine the portal
					int phase2 = 0;

					// We are now inside of a wedge...
					while (phase2 < 10)
					{
						phase2++;
						/*
						if (phase2 > 1)
						{

							
							phase2++;
							if (phase2 > 1)
							{
								static bool doneIt = false;
								if (!doneIt)
								{
									doneIt = true;
									//	g_debugQueue.clear();
									//gTrackingOn = true;
								//	CollideAndFindPoint(p1, p2, returnNormal, point1, point2);
									//gTrackingOn = false;
								//	return false;
								}
							}
							//static bool doneIt = false;
							/*
							if (!gTrackingOn && !doneIt)
							{
								doneIt = true;
								g_debugQueue.clear();
								gTrackingOn = true;
								CollideAndFindPoint(p1, q1, t1, p2, q2, t2, returnNormal, point1, point2);
								gTrackingOn = false;
								return false;
							}
							
						}
						*/

						// Compute normal of the wedge face
						n = (v2 - v1) % (v3 - v1);

						// Can this happen???  Can it be handled more cleanly?
						if (n->IsNearZero())
						{
							tool::show("Can this happen???  Can it be handled more cleanly?");
							return false;
						}

						n = n->normalized();

						// Compute distance from origin to wedge face
						float d = n ^ v1;

						// If the origin is inside the wedge, we have a hit
						if (d >= 0.f && !hit)
						{
							depth = d;
							if (returnNormal) 
								returnNormal = n;

							// Compute the barycentric coordinates of the origin
							float b0 = (v1 % v2) ^ v3;
							float b1 = (v3 % v2) ^ v0;
							float b2 = (v0 % v1) ^ v3;
							float b3 = (v2 % v1) ^ v0;

							float sum = b0 + b1 + b2 + b3;

							if (sum <= 0.f)
							{
								b0 = 0.f;
								b1 = (v2 % v3) ^ n;
								b2 = (v3 % v1) ^ n;
								b3 = (v1 % v2) ^ n;

								sum = b1 + b2 + b3;
							}

							float inv = 1.0f / sum;

							if (point1)
							{
								Vector^ p1 = (b0 * v01 + b1 * v11 + b2 * v21 + b3 * v31) * inv;
								point1 = p1;
							}

							if (point2)
							{
								Vector^ p2 = (b0 * v02 + b1 * v12 + b2 * v22 + b3 * v32) * inv;
								point2 = p2;
							}

							//globalPoint1 = point1 + (returnNormal->n*-depth);
							//globalPoint2 = point2 + (returnNormal->n*depth);
							// HIT!!!
							hit = true;
							//return true;  //not in the original implementation
						}

						// Find the support point in the direction of the wedge face
						supportCount++;
						Vector^ v41 = TransformSupportVert(A, -n) + t1;
						Vector^ v42 = TransformSupportVert(B, n) + t2;
						Vector^ v4 = v42 - v41;

						float delta = (v4 - v3) ^ n;
						float separation = -(v4 ^ n);

						// If the boundary is thin enough or the origin is outside the support plane for the newly discovered vertex, then we can terminate
						if (delta <= kCollideEpsilon || separation >= 0.0001f|| phase2 > 300)
						{
							if (returnNormal) 
								returnNormal = n;

							// MISS!!! (We didn't move closer)
							//static int32 maxPhase2 = 0;
							//static int32 maxCallCount = 0;
							//static float32 avg = 0.0f;
							//static int32 hitCount = 0;

							//static float32 avgSupportCount = 0.0f;
							/*
							if (hit)
							{
								hitCount++;

								avg = (avg * (hitCount - 1) + phase2) / hitCount;
								avgSupportCount = (avgSupportCount * (hitCount - 1) + supportCount) / hitCount;
								gAvgSupportCount = avgSupportCount;

								if (phase2 > maxPhase2)
								{
									maxPhase2 = phase2;
									maxCallCount = callCount;
								}
							}*/

							return hit;
						}

						// Compute the tetrahedron dividing face (v4,v0,v1)
						float d1 = (v4 % v1) ^ v0;

						// Compute the tetrahedron dividing face (v4,v0,v2)
						float d2 = (v4 % v2) ^ v0;

						// Compute the tetrahedron dividing face (v4,v0,v3)
						float d3 = (v4 % v3) ^ v0;

						if (d1 < 0.0001f)
						{
							if (d2 < 0.0001f)
							{
								// Inside d1 & inside d2 ==> eliminate v1
								v1 = v4;
								v11 = v41;
								v12 = v42;
							}
							else
							{
								// Inside d1 & outside d2 ==> eliminate v3
								v3 = v4;
								v31 = v41;
								v32 = v42;
							}
						}
						else
						{
							if (d3 < 0.0001f)
							{
								// Outside d1 & inside d3 ==> eliminate v2
								v2 = v4;
								v21 = v41;
								v22 = v42;
							}
							else
							{
								// Outside d1 & outside d3 ==> eliminate v1
								v1 = v4;
								v11 = v41;
								v12 = v42;
							}
						}
					}
				}
				return false;
			}

			Mesh^ A;
			Mesh^ B;

			float depth;
			Contact^ result;
		};
	}
}

