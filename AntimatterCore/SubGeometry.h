#pragma once
#include "Vector.h"
#include "Draw.h"
#include "Plane.h"
#include "Vertex.h"

//ray triangle intersection
//http://www.3dcpptutorials.sk/index.php?id=51

namespace Antimatter {
	namespace Games {

		public ref class Grid : Graphic
		{
		public:

			Grid()
			{

			}

			String^ visual = ".";
			int granularity = 1;
			Colour^ colour= Colour::White;
			int Scale = 10;
			float visualSize=2.f;
			bool DrawX = true;
			bool DrawY = false;
			bool DrawZ = true;
			bool DrawOrigin = true;

			void DrawGrid()
			{
				int length = Scale;
				for (int i = 0; i<length; i++)
				{
					if (DrawX)
					{
						Vector^ x = gcnew Vector((float)i, 0.f, 0.f);
						DrawLine(x, length, colour, visualSize, visual);
					}

					if (DrawY)
					{
						Vector^ y = gcnew Vector(0.f, (float)i, 0.f);
						DrawLine(y, length, colour, visualSize, visual);
					}

					if (DrawZ)
					{
						Vector^ z = gcnew Vector(0.f, 0.f, (float)i);
						DrawLine(z, length, colour, visualSize, visual);
					}
				}
			}

			void DrawLine(Vector^ direction,int length,Colour^ col, float scale, String^ visual)
			{
				Draw::TextInWorld(visual,direction, col, scale, scale,true);
				Draw::TextInWorld(visual,-direction, col, scale, scale,true);
			}
		};

		public ref class VertexInfo
		{
		public:
			VertexInfo()
			{
				pos = gcnew Vertex();
				normal = gcnew Vector();
			};

			VertexInfo(Vertex^ v)
			{
				pos = v;
				normal = gcnew Vector();
			};

			VertexInfo(Vertex^ v, Vector^ n)
			{
				pos = v;
				normal = n;
			};

			Vertex^ pos;
			Vector^ normal;
			Colour^ colour;
		};

		public ref class Face
		{
		public:
			Face()
			{

			};

			Face(unsigned int  a, unsigned int b, unsigned int c)
			{
				A = a;
				B = b;
				C = c;
			};

			unsigned int A;
			unsigned int B;
			unsigned int C;
			VertexInfo^ vertexA;
			VertexInfo^ vertexB;
			VertexInfo^ vertexC;
			Vector^ faceNormal = Vector::Zero;
			Plane^ plane;
			Vector^ edge0;
			Vector^ edge1;
			Vector^ centre;
			Transform^ transform;

			bool Encloses(Vector^ Point)
			{
				float D1 = -Vector::Dot(vertexA->normal, vertexA->pos);
				float D2 = -Vector::Dot(vertexB->normal, vertexB->pos);
				float D3 = -Vector::Dot(vertexC->normal, vertexC->pos);
				if (Vector::Dot(vertexA->normal, Point) + D1 < 0.0f) return false;
				if (Vector::Dot(vertexB->normal, Point) + D2 < 0.0f) return false;
				if (Vector::Dot(vertexC->normal, Point) + D3 < 0.0f) return false;

				return true;
			}

			Vector^ SetCentre()
			{
				return centre = (vertexA->pos + vertexB->pos + vertexC->pos)/3.f;
			}

			void Initialize()
			{
				plane = Plane::FromPoints(vertexA->pos,vertexB->pos,vertexC->pos)->Normalize();
				edge0 = vertexB->pos - vertexA->pos;
				edge1 = vertexC->pos - vertexA->pos;
			}

			Vector^ PointFromST(float s, float t)
			{
				return (1 - (s + t))*vertexA->pos + s*vertexB->pos + t*vertexC->pos;
			}

			Vector^ NormalFromST(float s, float t)
			{
				return (1 - (s + t))*vertexA->normal + s*vertexB->normal + t*vertexC->normal;
			}

			//http://www.3dcpptutorials.sk/index.php?id=51
			bool Intersect(Vector^% Origin, Vector^% Ray, Transform^ t,float MaxDistance, float% Distance, Vector^% Point)
			{
				Vector^tn = Vector::Cross(vertexB->pos - vertexA->pos, vertexC->pos - vertexA->pos)->normalized();
				float NdotR = -Vector::Dot(tn, Ray);
				float D = -Vector::Dot(tn,vertexA->pos);
				if (NdotR > 0.0f)
				{
					Distance = (Vector::Dot(tn, Origin) + D) / NdotR;

					if (Distance >= 0.0f && Distance < MaxDistance)
					{
						Point = Ray * Distance + Origin;

						return Encloses(Point);
					}
				}

				return false;
			}

			void draw(Transform^ t,Colour^ col)
			{
				Draw::DebugLine(vertexA->pos, vertexB->pos, col, t);
				Draw::DebugLine(vertexB->pos, vertexC->pos, col, t);
				Draw::DebugLine(vertexC->pos, vertexA->pos, col, t);
			}
		};

		public ref class PolyMesh
		{
		public:

			String^ name;
			List<Vertex^>^ points = gcnew List<Vertex^>();
			List<Vector^>^ normals = gcnew List<Vector^>();
			List<Face^>^ faces = gcnew List<Face^>();
			List<VertexInfo^>^ vertices = gcnew List<VertexInfo^>();
			List<Vector^>^ faceNormals = gcnew List<Vector^>();

			PolyMesh()
			{

			}

			VertexInfo^ GetVertex(unsigned int index)
			{
				VertexInfo^ v = gcnew VertexInfo();
				v->pos = points[index];
				v->normal = normals[index];
				vertices->Add(v);
				return v;
			}

			void SetFaceVertices()
			{
				for each(Face^ f in faces)
				{
					f->vertexA = GetVertex(f->A);
					f->vertexB = GetVertex(f->B);
					f->vertexC = GetVertex(f->C);
				}
			}

			void SetFaceNormals()
			{
				if (faces->Count != faceNormals->Count)
				{
					tool::debug(ConsoleColor::Red, name + " error: face count and normal count to do not match");
					return;
				}

				for (int i = 0; i < faceNormals->Count; i++)
				{
					faces[i]->faceNormal = faceNormals[i];
					faces[i]->SetCentre();
				}
			}

			void Init()
			{
				SetFaceVertices();
				SetFaceNormals();
			}

			void DrawFaces(Transform^ t,Colour^ col)
			{
				for each(Face^ f in faces)
				{
					f->draw(t, col);
				}
			}

			void DrawFaceNormals(Transform^ t,Colour^ c)
			{
				for(int i=0;i<faces->Count;i++)
				{
					Draw::DebugVector(t*faces[i]->centre->W(1.f), t*faces[i]->faceNormal->W(1.f), c);
				}
			}

			void DrawNormals(Transform^ t, Colour^ col)
			{
				for each(VertexInfo^ v in vertices)
				{
					Draw::DebugVector(v->pos, v->normal,col,t);
				}
			}
		};

		public ref class Ray
		{
		public:
			Ray()
			{

			};

			Ray(Vector^ beginning, Vector^ dir)
			{
				start = beginning;
				end = beginning+dir*distance;
				direction = start+dir;
				colour = gcnew Colour(1, 0, 0, 1);
				N = dir;
			}

			Ray(Vector^ beginning, Vector^ dir,Colour^ col)
			{
				start = beginning;
				end = beginning + dir*distance;
				direction = start+dir;
				colour = col;
				N = dir;
			}

			Vector^ start;
			Vector^ end;
			Vector^ direction;
			float maxDistance = FLT_MAX;
			float distance = maxDistance;
			bool hit;
			bool checkInterior;
			Vector^ hitPoint;
			Vector^ N;
			Colour^ colour;

			void Draw()
			{
				if (hit)
				{
					if (hitPoint != nullptr)
					{
						Draw::DebugLine(start, hitPoint, colour);
						Draw::DebugPointRaw(end, Colour::White, 0.5f);
					}
				}
				else
					Draw::DebugLine(start, start + N*maxDistance, colour);
			}

			//test
			void PlaneIntersect(Vector^ pos, Vector^ normal,Transform^ t)
			{
				Vector^ tNormal = (normal)->normalized();
				float NdotR = Vector::Dot(tNormal,N);
				float D = Vector::Dot(tNormal, pos-start);
				hit = false;
				hitPoint = nullptr;
				if (NdotR < 0 || checkInterior)
				{
					float Distance = (Vector::Dot(tNormal, pos-start)) / NdotR;
					if (Distance >= 0.0f && Distance < maxDistance)
					{
						end = -N->normalized() * -Distance + start;
						start->Display("start",2);
						hitPoint = end;
						hitPoint->Display("hit !");
						distance = -Distance;
						hit = true;
					}
				}
			}
		};
	}
}
