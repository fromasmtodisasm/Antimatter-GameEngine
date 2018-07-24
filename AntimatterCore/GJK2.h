#pragma once
#include "Vertex.h"
#include "Mesh.h"
#include "GJK2D.h"  //inclduing this for the evolve result enum.  Will build a gjk helper class instead
#include "Contact.h"

//hill climbing
//https://graphics.stanford.edu/courses/cs468-01-fall/Papers/cameron.pdf

//https://github.com/Discordia/gjk-epa/tree/master/src/collision

//epa optmisation
//https://gist.github.com/jesstelford/7829992

//http://www.goblinphysics.com/documentation/files/src_classes_Collision_GjkEpa2.js.html

//clipping
//http://www.dyn4j.org/2011/11/contact-points-using-clipping/
//
//gjk 2d and 3d
//http://wiki.roblox.com/index.php?title=User:EgoMoose/Articles/GJK_collision_detection
//https://uk.mathworks.com/matlabcentral/fileexchange/57907-fast-3d-collision-detection-gjk-algorithm


namespace Antimatter{
	namespace Games {

		public ref class GJKDistance
		{
		public:

			ref class GJKFeature
			{
			public:
				GJKFeature()
				{
					a = -1;
					b = -1;
					c = -1;
					count = 0;
				}

				GJKFeature(int A)
				{
					a = A;
					b = -1;
					c = -1;
					count = 1;
				}

				GJKFeature(int A,int B)
				{
					a = A;
					b = B;
					c = -1;
					count = 2;
				}

				GJKFeature(int A, int B, int C)
				{
					a = A;
					b = B;
					c = C;
					count = 3;
				}

				int GetCount()
				{
					count = 0;
					if (a > -1)
						count++;
					if (b > -1)
						count++;
					if (c > -1)
						count++;
					return count;
				}

				int a;
				int b;
				int c;
				int count;
				Vector^ edgeNormal;
			};

			ref class GJKTriangle : List<SupportPoint^>
			{
			public:
				GJKTriangle()
				{

				}

				GJKTriangle(GJKTriangle^ gjk_tri)
				{
					this->Clear();
					Add(gjk_tri[0]);
					Add(gjk_tri[1]);
					Add(gjk_tri[2]);
				}

				GJKTriangle(SupportPoint^ A, SupportPoint^ B, SupportPoint^ C)
				{
					Add(A);
					Add(B);
					Add(C);
					colour = Colour::Purple;
				}

				Colour^ colour;
				Vector^ normal1;
				Vector^ normal2;
				Vector^ normal3;

				void GetEdgeNormals()
				{
					// subtract the two to get the edge vector
					Vector^ edge1= this[1] - this[0];
					Vector^ edge2= this[2] - this[1];
					Vector^ edge3= this[0] - this[2];

					// 1 Surface normal
					Vector^ normal = GetNormal()->normalized();
					// 3 Edge normals
					normal1 = Vector::Cross(edge1, normal)->normalized();
					//Draw::DebugLine(this[0], this[0] + normal1, Colour::Yellow);

					normal2 = Vector::Cross(edge2, normal)->normalized();
					//Draw::DebugLine(this[1], this[1] + normal2, Colour::Yellow);

					normal3 = Vector::Cross(edge3, normal)->normalized();
					//Draw::DebugLine(this[2], this[2] + normal3, Colour::Yellow);

					//this[0]->DrawAs("0", Colour::Green);
					//this[1]->DrawAs("1", Colour::Green);
					//this[2]->DrawAs("2", Colour::Green);
				}

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

					Vector^ v01 = this[1] - this[0];
					Vector^ v02 = this[2] - this[0];
					//having to flip the normal: is this acceptable?
					return -v01->cross(v02)->normalized();
				}

				GJKFeature^ ClosesetFeature(Vector^ point)
				{
					GJKFeature^ gf = gcnew GJKFeature();
					Vector^ normal = GetNormal()->normalized();
					
					float v_d = -FLT_MAX;
					float e_d = -FLT_MAX;
					int closest_vert = 0;
					for (int i = 0; i < this->Count; i++)
					{
						int j = i + 1;
						if (j >= this->Count)
							j = 0;
						float d = this[i]->dot(point);
						Vector^ en= Vector::Cross(this[j] - this[i], normal)->normalized();
						float ed = en->dot(point);
						if (d > v_d)
						{
							v_d = d;
							closest_vert = i;
						}

						if (ed > e_d)
						{
							e_d = ed;
							gf->a = i;
							gf->b = j;
							gf->edgeNormal = en;
						}
					}
					if (v_d > e_d)
					{
						//gf->a = closest_vert;
						//gf->b = -1;
					}
					return gf;
				}

				void DrawFeature(GJKFeature^ feature,Colour^ col)
				{
					if (feature == nullptr)
						return;
					int count = feature->GetCount();

					if (count == 0)
						return;
					if (count == 1)
						this[feature->a]->DrawAsPointRaw(col, 0.2f);
					if (count == 2)
						this[feature->a]->DrawTo(this[feature->b],col);
					if (count == 3)
						Draw(col);

				}

				GJKTriangle^ EvolveFace(SupportPoint^ sp)
				{
					GJKTriangle^ new_tri = gcnew GJKTriangle(this);
					int el = new_tri->GetElimationIndex(sp);

					new_tri[el] = sp;
					return new_tri;

				}

				int GetElimationIndex(Vector^ direction)
				{
					float c0 = direction->dot(this[0]);
					float c1 = direction->dot(this[1]);
					float c2 = direction->dot(this[2]);
					float point_dist = 0.f;
					//tool::Show(c0,c1,c2);
					int elimation_index = -1;
					if (c0 > c1)
					{
						elimation_index = 0;
						point_dist = c0;

					}
					else
					{
						elimation_index = 1;
						point_dist = c1;
					}
					if (c2 > point_dist)
					{
						elimation_index = 2;
						//point_dist = c2->Length();
					}
					return elimation_index;
				}

				Vector^ GetNormal()
				{
					return Vector::Cross(this[1] - this[0], this[2] - this[0]);
				}

				Vector^ Centre()
				{
					return (this[0] + this[1] + this[2]) / 3.f;
				}

				SupportPoint^ ClosestVertex(Vector^ point)
				{
					float g_max = FLT_MAX;
					SupportPoint^ minVertex = nullptr;
					for each(SupportPoint^ sp in this)
					{
						//can probably just use the world length here
						float sp_d = Vector::Distance(point, sp);
						if (sp_d < g_max)
						{
							g_max = sp_d;
							minVertex = sp;
						}
					}
					return minVertex;
				}

				void Draw(Colour^ col)
				{
					if (this == nullptr)
						return;
					Draw::Triangle(this[0], this[1], this[2], col);
				}

				void Debug(Colour^ col, Colour^ col2)
				{
					Vector^ c_ = Centre();
					Vector^ n = Normal();
					Colour^ simplexColour = col2;
					Colour^ f_ = col*0.8f;
					Draw(simplexColour);
					Draw::DebugLine(c_, c_ + n, f_, Colour::Yellow);
					Draw::DebugLine(c_, this[0], f_, simplexColour);
					Draw::DebugLine(c_, this[1], f_, simplexColour);
					Draw::DebugLine(c_, this[2], f_, simplexColour);
				}
			};
			GJKDistance()
			{
				origin = Vector::Zero;
			}

			Mesh^ A;
			Mesh^ B;

			GJKTriangle^ simplex = gcnew GJKTriangle();
			GJKFeature^ closestFeature = gcnew GJKFeature();
			List<SupportPoint^>^ hull = gcnew List<SupportPoint^>();
			Vector^ searchDirection;
			Vector^ initialDirection;
			Vector^ origin;
			GJKTriangle^ closestFace;
			GJKTriangle^ nextFace;
			Contact^ result;
			bool intersecting;
			bool pointInTriangle;
			Vector^ lastElimated;
			SupportPoint^ closestPoint = nullptr;
			SupportPoint^ furthestSupport = nullptr;
			SupportPoint^ nextSupport = nullptr;
			SupportPoint^ newClosest = nullptr;
			SupportPoint^ minVertex = nullptr;
			int iteration_limit = -1;


			SupportPoint^ calculateSupport(Vector^ direction)
			{
				Vector^ oppositeDirection = direction*-1;
				Vertex^ va = A->Support(direction);
				Vertex^ vb = B->Support(oppositeDirection);
				SupportPoint^ sp = gcnew SupportPoint(va - vb, va, vb);
				return sp;
			}

			void barycentric(Vector^ p, Vector^ a, Vector^ b, Vector^ c, float% u, float% v, float% w) {
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

			
			bool UpdateResult(float currentDist_, List<SupportPoint^>^ tri,Vector^ point)
			{
				float u;
				float v;
				float w;

				barycentric(point, simplex[0], simplex[1], simplex[2], u, v, w);


				if (float::IsNaN(u) || float::IsNaN(v) || float::IsNaN(w))
					return false;

				if (fabs(u) < 0.f || fabs(v) < 0.f || fabs(w) < 0.f)
					return false;
				/*
				if (u < -1.f)
					u = -1.f;
				if (u > 1.f)
					u = 1.f;

				if (v < -1.f)
					v = -1.f;
				if (v > 1.f)
					v = 1.f;

				if (w < -1.f)
					w = -1.f;
				if (w > 1.f)
					w = 1.f;
					
					*/
					
				if (u > 0.f && v > 0.f && w > 0.f && u + v + w <= 1.f)
				{
					pointInTriangle = true;
					tool::debugYellow("inside triangle");
				}

				result = gcnew Contact(A, B);

				result->point1 = (simplex[0]->vertexA * u) +
					(simplex[1]->vertexA* v) +
					(simplex[2]->vertexA * w);
				//two methods for computing the next point
				//result->point2 = result->point1 + result->normal*depth;
				result->point2 = (simplex[0]->vertexB * u) +
					(simplex[1]->vertexB* v) +
					(simplex[2]->vertexB * w);
				result->localA = glm::inverse(A->gxtransform->staticMatrix)*result->point1;
				result->localB = glm::inverse(B->gxtransform->staticMatrix)*result->point2;

				result->r1 = result->point1 - A->Position;
				result->r2 = result->point2 - B->Position;
				result->depth = currentDist_;
				result->normal = result->point2 - result->point1;
				intersecting = true;
				return true;
			}
			

			Vector^ closestPointToOrigin(Vector^ a, Vector^ b)
			{
				Vector^ AB = b - a;
				Vector^ A0 = origin - a;
				float d1 = A0->dot(AB);
				float d2 = AB->dot(AB);
				if (d1 == 0.f || d2 == 0.f)
				{
					tool::show("Handle 0 length case in Point to origin");
					return nullptr;
				}
				float t = d1 / d2;

				Vector^ point = AB*t + a;
				return point;
			}

			float closestDistanceToOrigin(Vector^ a, Vector^ b)
			{
				Vector^ AB = b - a;
				Vector^ A0 = origin - a;
				float d1 = A0->dot(AB);
				float d2 = AB->dot(AB);
				if (d1 == 0.f || d2 == 0.f)
				{
					//tool::Show("Handle 0 length case in Point to origin");
					return 0;
				}
				float t = d1 / d2;

				return t;
			}

			void DrawSimplex(Colour^ col,float size)
			{
				Vector::Zero->DrawAs(". origin", Colour::White,0.5f);
				if (pointInTriangle)
					col = Colour::Green;
				for (int i = 0; i < simplex->Count; i++)
				{
					int j = i + 1;
					if (j >= simplex->Count)
					{
						j = 0;
					}
					
					Draw::DebugLine(simplex[i], simplex[j], col);
					simplex[i]->DrawAs(i.ToString(), Colour::White, size);
				}
				if (searchDirection != nullptr && simplex->Count > 0)
					Draw::DebugLine(simplex[0], simplex[0] + searchDirection, Colour::Yellow,gcnew Colour(0.f,1.f,1.f,0.f));
				//if (lastElimated != nullptr)
				//	lastElimated->DrawAs("Last Eliminated",Colour::Red, 0.5f);
				if (closestPoint != nullptr)
				{
					origin->DrawTo(closestPoint, Colour::LightBlue*0.5f);
					closestPoint->DrawAsPointRaw(Colour::New, 0.2f);
				}
				if (minVertex != nullptr)
					minVertex->DrawAs("Min Vertex",Colour::New, 0.5f);
				//if (nextSupport != nullptr)
				//	nextSupport->DrawAs("Next Support", Colour::Yellow, 0.5f);

				//if (furthestSupport != nullptr)
				//	furthestSupport->DrawAs("Furthest vertex", Colour::LightBlue, 0.7f);

				Vector^ centre = A->Position - B->Position;
				centre->DrawAs("Centre", Colour::White+(Colour::Yellow*0.5f), 0.7f);
				if (initialDirection != nullptr)
					centre->DrawTo(initialDirection->normalized(), Colour::Green,gcnew Colour(0.f,1.f,0.f,0.f));


				if (result != nullptr)
				{
					result->point1->DrawAs(". A", Colour::White);
					result->point2->DrawAs(". B", Colour::LightBlue);
					result->point1->DrawAsPointRaw( Colour::White,0.1f);
					result->point2->DrawAsPointRaw(Colour::LightBlue,0.1f);
				}
				if (closestFeature != nullptr)
					simplex->DrawFeature(closestFeature, Colour::Green);
				//if (newClosest != nullptr)
				//	newClosest->DrawAs("Next Closest", Colour::Green, 0.7f);
			}

			List<SupportPoint^>^ BuildHull()
			{
				List<SupportPoint^>^ min_hull = gcnew List<SupportPoint^>();
				List<Vertex^>^ pb = B->GetTransformedPoints();
				for each(Vertex^ va in A->GetTransformedPoints())
				{
					for each(Vertex^ vb in pb)
					{
						SupportPoint^ sp = gcnew SupportPoint(va - vb, va, vb);
						min_hull->Add(sp);
					}
				}
				return min_hull;
			}

			void DrawHull(Colour^ col,float size)
			{
				//hull->Clear();
				hull = BuildHull();
				for each(SupportPoint^ sp in hull)
				{
					sp->DrawAsPointRaw(col, size);
				}
			}

			Vector^ GetHalfSpace()
			{
				return (A->Position + B->Position)* 0.5f;
			}

			bool Test2(Mesh^ a, Mesh^ b)
			{
				A = a;
				B = b;
				intersecting = false;
				result = nullptr;
				simplex->Clear();
				hull->Clear();
				hull = BuildHull();
				int iterations = 0;

				origin->DrawAs(". Origin", Colour::White);

				searchDirection = origin - (A->Position - B->Position);
				SupportPoint^ p0 = calculateSupport(searchDirection);
				simplex->Add(p0);

				SupportPoint^ p1 = calculateSupport(-searchDirection);
				simplex->Add(p1);

				searchDirection = closestPointToOrigin(simplex[0], simplex[1]);
				SupportPoint^ p2 = calculateSupport(searchDirection);
				simplex->Add(p2);

				Vector^ n = simplex->GetNormal();

				//try and get the normal to point towards the origin
				//float towards_origin = n->dot(simplex[0] - origin);

				searchDirection = n;
				closestFeature = nullptr;

				int i = 0;
				GJKTriangle^ next_tri = nullptr;
				SupportPoint^ cv = nullptr;
				SupportPoint^ p4 = nullptr;
				String^ status = "Searching hull";
				SupportPoint^ test_point = gcnew SupportPoint();
				while (i < iteration_limit)
				{
					
					//searchDirection = closestPointToOrigin(simplex[0], simplex[1]);
					//get a new point;
					
					//searchDirection = simplex->GetNormal();
					SupportPoint^ p3 = calculateSupport(searchDirection);
					Vector^ tri_n = searchDirection;
					
					float c_p_new = p3->dot(searchDirection);
					float c_p_old = simplex[0]->dot(searchDirection);

					tri_n = simplex->GetNormal();
					if (tri_n->length() < 0.001f)
						tool::debugYellow("Handle edge case of minkowski hull");
					float new_dist = tri_n->dot(p3 - origin) / tri_n->dot(tri_n);
					float new_dot = tri_n->dot(simplex[0] - origin);
					//tool::Show(new_dist);
					
					if (c_p_new - c_p_old < 0.000001f)
					{
						status = "Reached hull";
						float proj_dist = searchDirection->dot(simplex[0] - origin) / searchDirection->dot(searchDirection);
						closestPoint = gcnew SupportPoint(searchDirection*proj_dist);
						bool updated = UpdateResult(0.f, simplex, closestPoint);
						if (!pointInTriangle)
						{
							//cv = simplex->ClosestVertex(closestPoint);
							simplex->GetEdgeNormals();
							closestFeature = simplex->ClosesetFeature(closestPoint);
							if (closestFeature->GetCount() == 2)
							{
								searchDirection = closestFeature->edgeNormal;

							}
							//closestPoint = simplex[cf->a];
							//searchDirection = closestPoint - cv;
							continue;
						}

					}
					else {
						status = "Finding hull";
						
						//searchDirection = simplex->GetNormal();
						//next_tri = simplex->EvolveFace(p3);
						//simplex = next_tri;
						//searchDirection = simplex->GetNormal();
						//GetHalfSpace()->DrawAs("Finding Hull", Colour::Orange);
					}
					
					next_tri = simplex->EvolveFace(p3);
					simplex = next_tri;
					
					searchDirection = simplex->GetNormal();
					float proj_dist = searchDirection->dot(simplex[0] - origin) / searchDirection->dot(searchDirection);
					closestPoint = gcnew SupportPoint(searchDirection*proj_dist);
	
					//searchDirection = (closestPoint - cv);
					//p4 = calculateSupport(searchDirection);

					i++;
				}
				simplex->Draw(Colour::Purple);
				//next_tri->Draw(Colour::Green);
				closestPoint->DrawAsPointRaw(Colour::New, 0.2f);
				origin->DrawTo(closestPoint, Colour::LightBlue);
				simplex[0]->DrawTo(simplex[0] + searchDirection->normalized(), Colour::Yellow, Colour::Yellow - gcnew Colour(0.f, 0.f, 0.f, 1.f));
				simplex[0]->DrawTo(simplex[0] + simplex->GetNormal()->normalized(), Colour::Green, Colour::Green - gcnew Colour(0.f, 0.f, 0.f, 1.f));
				
				//cv->DrawAs("Closest Vertex", Colour::White, 0.2f);
				GetHalfSpace()->DrawAs(status, Colour::Orange);
				p4->DrawAs("Next Point", Colour::Yellow, 0.5f);
				p4->DrawAsPointRaw(Colour::Yellow, 0.07f);

				if (closestFeature != nullptr)
					simplex->DrawFeature(closestFeature,Colour::LightBlue);

				return false;
			}

			bool Test(Mesh^ a, Mesh^ b)
			{
				A = a;
				B = b;
				intersecting = false;
				result = nullptr;
				simplex->Clear();
				hull->Clear();
				hull = BuildHull();
				int iterations = 0;
				int elimation_index = -1;
				float old_dist = 0.f;
				float tolerance = 0.f;
				pointInTriangle = false;
				SupportPoint^ p0 = nullptr;
				SupportPoint^ p1 = nullptr;
				SupportPoint^ p2 = nullptr;
				SupportPoint^ p3 = nullptr;
				nextSupport = nullptr;
				minVertex = nullptr;

				searchDirection = origin-(A->Position - B->Position);
				p0 = calculateSupport(searchDirection);
				initialDirection = searchDirection;
				simplex->Add(p0);

				//searchDirection = B->Position - A->Position;
				p1 = calculateSupport(-searchDirection);
				simplex->Add(p1);

				searchDirection = closestPointToOrigin(simplex[0],simplex[1]);
				p2 = calculateSupport(searchDirection);
				simplex->Add(p2);
							
				bool reached_edge = false;
				int iter = 0;
				String^ perp_check;
				while (!reached_edge)
				{
					iter++;
					//get the normal the current simplex face
					Vector^ tri_normal = Vector::Cross(simplex[1] - simplex[0], simplex[2] - simplex[0]);
				
					searchDirection = tri_normal;
					//searchDirection = closestPointToOrigin(simplex[0], simplex[1]);
					//get a new point;
					SupportPoint^ p3 = calculateSupport(searchDirection);
					float c_p_new = closestDistanceToOrigin(simplex[0], p3);
					float c_p_old = closestDistanceToOrigin(simplex[0], simplex[1]);
					if (c_p_new < c_p_old)
						perp_check = c_p_new.ToString();
					else
					{
						//perp_check = "Found Hull";
						//return false;
					}
					//use the direction vector of the new point to the origin to calculate which is the deepest
					//support point to remove
					elimation_index = GetElimationIndex(p3-origin);

					if (elimation_index > -1)
					{
									
						lastElimated = simplex[elimation_index];//debug
						simplex[elimation_index] = p3;  //assign the support point.  Winding needs to be handled properly
						//check next point;
						//get normal of the enwly updated face
						Vector^ tri_n = Vector::Cross(simplex[1] - simplex[0], simplex[2] - simplex[0]);
								
						searchDirection = tri_n;
								
						//get the next support point
						nextSupport = calculateSupport(searchDirection);
									
						//check it's direction to the origin  we probably need tp check this
						//against the previous value to check progress towards the origin
						Vector^ vnew = origin - simplex[0];
						Vector^ nextDirection = searchDirection;
						float new_dist = searchDirection->dot(simplex[0] - origin) / nextDirection->dot(nextDirection);
						float new_dot = nextDirection->dot(simplex[0] - origin);
						//tool::Show(new_dist);
						closestPoint = gcnew SupportPoint(nextDirection*new_dist);
					
						if (new_dot < 0.f)
						{

							perp_check = "Found hull !";
							bool updated=UpdateResult(0.f, simplex, closestPoint);
							if (!pointInTriangle)
							{
								float g_max = FLT_MAX;
								for each(SupportPoint^ sp in simplex)
								{
									float sp_d = Vector::Distance(closestPoint,sp);
									if (sp_d < g_max)
									{
										g_max = sp_d;
										minVertex = sp;
									}
								}
								//new search direction
								//min_vertex - closestPoint
							}
							if (updated)
							{
								float final_distance =Vector::Distance(result->point1, result->point2);
								if (final_distance < 0.01f)
								{

									perp_check = "Found intersection !";
									intersecting = true;
								}
							}
							//return true;
						}
						else
						{
							perp_check = "Found Penetration!";
							//searchDirection = -searchDirection;  //try new search direction
										
							//handle case of intersection
						}

						//debug stuff
						if (iter == iteration_limit)
						{
							Vector^ half = A->Position + (B->Position - A->Position)*0.5f;
							half->DrawAs(perp_check, Colour::New);
						}
					}

					//iteration limit
					if (iter > iteration_limit)
					{
						return false;

					}

				}
				return false;
			}

			int GetElimationIndex(Vector^ direction)
			{
				float c0 = direction->dot(simplex[0] - origin);
				float c1 = direction->dot(simplex[1] - origin);
				float c2 = direction->dot(simplex[2] - origin);
				float point_dist = 0.f;
				//tool::Show(c0,c1,c2);
				int elimation_index = -1;
				if (c0 > c1)
				{
					elimation_index = 0;
					point_dist = c0;

				}
				else
				{
					elimation_index = 1;
					point_dist = c1;
				}
				if (c2 > point_dist)
				{
					elimation_index = 2;
					//point_dist = c2->Length();
				}
				return elimation_index;
			}
		};
	}
}

