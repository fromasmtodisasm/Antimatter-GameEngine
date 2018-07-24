#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Vector.h"
#include "Transform.h"
#include "glaxVAO.h"
#include "glaxRenderer.h"
#include "XFileLoader.h"
#include "LineGraphic.h"

//half edge data struct
//http://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtmlhttp://www.flipcode.com/archives/The_Half-Edge_Data_Structure.shtml

using namespace System::Collections::Generic;

namespace Antimatter {
	namespace Games {

		public ref class Shape
		{
		public:
			Shape()
			{
				vao = NULL;
			};

			float volume;
			float mass;
			float radius;
			Vector^ centreOfMass;
			Vector^ inertiaDiag;
			String^ name;
			String^ filePath;
			List<float>^ Data = gcnew List<float>();
			List<Vertex^>^ points = gcnew List<Vertex^>();
			List<unsigned int>^ IndexList = gcnew List<unsigned int>();
			List<TextureUV^>^ UVs = gcnew List<TextureUV^>();
			List<Vector^>^ FaceNormals = gcnew List<Vector^>();
			List<float>^ ColourData = gcnew List<float>();
			List<Vector^>^ Normals = gcnew List<Vector^>();
			List<Face^>^ Faces = gcnew List<Face^>();
			cli::array <float>^ interLeavedData;
			IntPtr^ dataPointer;
			IntPtr^ indexPointer;
			unsigned int ID;
			glx::VAO* vao; //only native object.  Please dispose or convert to managed class
			PolyMesh^ polymesh;
			bool Debug = false;
			static List<Shape^>^ Shapes = gcnew List<Shape^>();
			static unsigned int _ShapeIndex;

			//generate a mesh from a xfile
			static Shape^ MeshFromXFile(String^ s)
			{
				//create an xfile from the path
				Shape^ shape = gcnew Shape();
				shape->SetFile(s);
				XFileLoader^ xfile = gcnew XFileLoader(s, false);
				cli::array<float>^ dat = xfile->BuildInterleaveData();	//interleave the data for the buffer
				int datcount = dat->Length;		 //get the length of the interleaved data
				pin_ptr<float> data = &dat[0];
				int indcount = xfile->indexList->Count;	   //and the indices
				pin_ptr<unsigned int> indices = &xfile->indexList->ToArray()[0];

				//send the data to opengl and get a vbo id
				//this pointer is a bit unnecessary.  Copy the id data instead
				glx::VAO* vbo = glx::Render::Default->BufferPrimitive(data, datcount, indices, indcount);

				for each(Vertex^ p in xfile->vertices)
				{
					shape->points->Add(p);
				}

				shape->FaceNormals = xfile->faceNormals;
				shape->Normals = xfile->normals;
				shape->UVs = xfile->UVs;
				shape->IndexList = xfile->indexList;
				shape->vao = vbo;
				shape->Faces = xfile->faces;
				shape->polymesh = gcnew PolyMesh();
				shape->polymesh->points = shape->points;
				shape->polymesh->normals = shape->Normals;
				shape->polymesh->faces = shape->Faces;
				shape->polymesh->faceNormals = xfile->faceNormals;
				shape->polymesh->name = shape->name;
				shape->polymesh->Init();
				shape->radius = shape->GetRadius();
				dat = nullptr;
				data = nullptr;
				indices = nullptr;

				return shape;
			}

			cli::array<float>^ BuildInterleaveData()
			{
				if (Data->Count == 0)
				{
					tool::show("error, no vertex data");
					return nullptr;
				}

				if (Normals->Count == 0)
				{
					tool::show("error, no normal data");
					return nullptr;
				}

				if (UVs->Count == 0)
				{
					tool::debug(" no UV data");
				}
				else
				{
					tool::debug("XFile:: GetMesh:" + Path::GetFileNameWithoutExtension(filePath) + " UVs " + UVs->Count);
				}

				List<float>^ dat = gcnew List<float>();
				int vertIndex = 0;

				if (Debug)
				{
					tool::debug(" interleave vertex count: " + Data->Count, " interleave normal count: " + Normals->Count);
				}

				for (int i = 0; i < Data->Count / 3; i++)
				{
					dat->Add(Data[vertIndex]);
					dat->Add(Data[vertIndex + 1]);
					dat->Add(Data[vertIndex + 2]);
					dat->Add(Normals[i]->x);
					dat->Add(Normals[i]->y);
					dat->Add(Normals[i]->z);

					if (UVs->Count > 0)
					{
						dat->Add(UVs[i]->U);
						dat->Add(UVs[i]->V);
					}
					else
					{
						dat->Add(0.5f);
						dat->Add(0.5f);
					}
					vertIndex = vertIndex + 3;
				}

				if (Data->Count != Normals->Count)
					tool::debug(filePath + "\nerror, vertex count and normal count do not match" + "\n\nData count: " + Data->Count + "\nNormal count: " + IndexList->Count);
				interLeavedData = dat->ToArray();

				return interLeavedData;
			}

			void DrawBounds(Vector^ maxBounds, Vector^ minBounds,Colour^ col)
			{
				Vector^ max = maxBounds;
				Vector^ min = minBounds;
				Vector^ a = min;
				Vector^ b = gcnew Vector(min->x, min->y, max->z);
				Vector^ c = gcnew Vector(max->x, min->y, max->z);
				Vector^ d = gcnew Vector(max->x, min->y, min->z);

				gcnew LineGraphic(a, b, col);
				gcnew LineGraphic(b, c, col);
				gcnew LineGraphic(c, d, col);
				gcnew LineGraphic(d, a, col);

				Vector^ am = max;
				Vector^ bm = gcnew Vector(max->x, max->y, min->z);
				Vector^ cm = gcnew Vector(min->x, max->y, min->z);
				Vector^ dm = gcnew Vector(min->x, max->y, max->z);

				gcnew LineGraphic(am, bm, col);
				gcnew LineGraphic(bm, cm, col);
				gcnew LineGraphic(cm, dm, col);
				gcnew LineGraphic(dm, am, col);

				gcnew LineGraphic(a, cm, col);
				gcnew LineGraphic(b, dm, col);
				gcnew LineGraphic(c, am, col);
				gcnew LineGraphic(d, bm, col);
			}
			
			void ComputeBounds(Transform^ t,Vector^% min,Vector^% max) 
			{
				// TODO : Only works if the local origin is inside the mesh
				//        => Make it more robust (init with first vertex of mesh instead)

				min = t->Position;
				max = t->Position;

				// For each vertex of the mesh
				for (int i = 0; i<points->Count; i++) {

					Vector^ v  = t*points[i]->W(1.f);
					if (v->x > max->x) max->x = v->x;
					if (v->x < min->x) min->x = v->x;

					if (v->y > max->y) max->y = v->y;
					if (v->y < min->y) min->y = v->y;

					if (v->z > max->z) max->z = v->z;
					if (v->z < min->z) min->z = v->z;
				}
			}

			float GetVolume()
			{
				// count is the number of triangles (tris) 
				float  vol = 0;
				for (int i = 0; i<Faces->Count; i++)  // for each triangle
				{
					Vector^ A = points[Faces[i]->A];
					Vector^ B = points[Faces[i]->B];
					Vector^ C = points[Faces[i]->C];
					//glm::mat3 mat = glm::mat3(A->ToGLM(), B->ToGLM(), C->ToGLM());
					vol += Determinant(A->glm(), B->glm(), C->glm());  //divide by 6 later for efficiency
				}
				return volume = vol / 6.0f;  // since the determinant give 6 times tetra volume
			}

			Vector^ GetCenterOfMass()
			{
				Vector^ com = Vector::Zero;
				float  v = 0.f; 
				float vol = volume;
				for (int i = 0; i<Faces->Count; i++)  // for each triangle
				{
					Vector^ A = points[Faces[i]->A];
					Vector^ B = points[Faces[i]->B];
					Vector^ C = points[Faces[i]->C];
					com += vol * (A + B + C);  // divide by 4 at end
					v += vol;
				}
				com /= v*4.0f;

				return centreOfMass = com;
			}

			
			float Determinant(Vector^ a,Vector^ b,Vector^ c) 
			{ 
				return a->x*(b->y*c->z - c->y*b->z) + a->y*(b->z*b->x -c->z*b->x) + a->z*(b->x*c->y - c->x*b->y); 
			}

			void SetFile(String^ path)
			{
				filePath = path;
				name = Path::GetFileNameWithoutExtension(path);
			}

			static void AddShape(Shape^ newshape)
			{
				newshape->ID = _ShapeIndex++;
				Shapes->Add(newshape);
			}

			static bool ContainsShape(String^ path)
			{
				for each(Shape^ s in Shapes)
				{
					if (s->filePath == path)
					{
						return true;
					}
				}
				return false;
			}

			void debugMe(String^ s)
			{
				tool::debug(s, "name: " + name,
					"Path: " + filePath,
					"data Count: " + Data->Count,
					"index Count: " + IndexList->Count,
					"normal count: " + Normals->Count,
					"interleaved data length: " + interLeavedData->Length);
			}

			cli::array<Vector^>^ UpdatedTransform(Transform^ parent)
			{
				cli::array<Vector^>^ p = gcnew cli::array<Vector^>(points->Count);
				glm::mat4 modelMatrix = parent->gxtransform->GetWorldMatrix();

				for (int i = 0; i < points->Count; i++)
				{
					glm::vec4  m = modelMatrix* points[i]->ToGLM4(1.f);
					p[i] = gcnew Vector(m);
				}
				return p;
			}

			Vector^ GetFarthestPointInDirection(Transform^ transform, Vector^ dir)
			{
				if (points == nullptr || points->Count == 0)
					return nullptr;

				Vector^ returnVector;
				//convert in world coordinates
				glm::mat4 modelMatrix = transform->gxtransform->staticMatrix;
				glm::vec4  m = modelMatrix*points[0]->ToGLM4(1);
				Vector^ worldVertex = gcnew Vector(m);

				float maxDot = worldVertex->dot(dir);
				returnVector = worldVertex;

				for (int i = 1; i < points->Count; i++)
				{
					//convert in world coordinates
					m = modelMatrix * points[i]->ToGLM4(1);
					worldVertex = gcnew Vector(m);
					float dot = worldVertex->dot(dir);
					//find biggest dot product
					if (dot > maxDot)
					{
						maxDot = dot;
						returnVector = worldVertex;
					}
				}
				return returnVector;
			}

			float GetRadius()
			{
				float furthest = 0.f;
				for each(Vector^ v in points)
				{
					float dist = v->DistanceSquared(Vector::Zero, v);
					if (dist > furthest)
						furthest = dist;
				}
				return furthest;
			}

			static Shape^ FromXFile(String^ path)
			{
				for each(Shape^ s in Shapes)
				{
					if (s->filePath == path)
					{
						return s;
					}
				}

				Shape^ news = MeshFromXFile(path);
				AddShape(news);
				return news;
			}

			Vector^ SupportMapping(Vector^ v)
			{
				unsigned int n_points = points->Count;
				if (n_points == 0)
					return nullptr;

				unsigned int best = 0;
				float best_dot = Vector::Dot(points[0], v);

				for (unsigned int i = 1; i < n_points; i++)
				{
					float d = Vector::Dot(points[i], v);
					if (d > best_dot)
					{
						best = i;
						best_dot = d;
					}
				}

				return points[best];
			}
		};

		//http://clb.demon.fi/MathGeoLib/nightly/docs/AABB_summary.php
		public ref class AABB
		{
		public:
			Vector^ centre;
			Vector^ extents;
			bool debug;

			AABB()
			{
				debug = false;
			}

			AABB(List<Vertex^>^ points)
			{
				AABB^ a = AABB::Build(points);
				centre = a->centre;
				extents = a->extents;
				extents *= 2.f;
				debug = true;
			}

			AABB(Shape^ s,Vector^ scale)
			{
				AABB^ a = AABB::Build(s->points);
				centre = a->centre;
				extents = a->extents;
				extents *= (scale*1.5f);
				debug = false;
			}

			AABB(const AABB% aabb)
			{
				extents = aabb.extents;
				centre = aabb.centre;
			};

			void Debug(String^ text)
			{
				Console::WriteLine("\n" + text + "\nAABB::");
				centre->debugVec("centre");
				extents->debugVec("extents");
				GetMin()->debugVec("Min");
				GetMax()->debugVec("Max");
			}

			static void Draw(Vector^ min,Vector^ max,Colour^ colour)
			{
				float leftX = min->x;
				float bottomY = min->y;
				float backZ = min->z;

				float rightX = max->x;
				float frontZ = max->z;
				float topY = max->y;

				Vector^ topbackleft = gcnew Vector(leftX, topY, backZ);
				Vector^ topbackright = gcnew Vector(rightX, topY, backZ);
				Vector^ topfrontright = gcnew Vector(rightX, topY, frontZ);
				Vector^ topfrontleft = gcnew Vector(leftX, topY, frontZ);

				//bottom face
				Vector^ bottombackleft = gcnew Vector(leftX, bottomY, backZ);
				Vector^ bottombackright = gcnew Vector(rightX, bottomY, backZ);
				Vector^ bottomfrontright = gcnew Vector(rightX, bottomY, frontZ);
				Vector^ bottomfrontleft = gcnew Vector(leftX, bottomY, frontZ);
				//draw top face
				Draw::DebugLine(topbackleft, topbackright, colour);
				Draw::DebugLine(topbackright, topfrontright, colour);
				Draw::DebugLine(topfrontright, topfrontleft, colour);
				Draw::DebugLine(topfrontleft, topbackleft, colour);
				//draw bottom fae
				Draw::DebugLine(bottombackleft, bottombackright, colour);
				Draw::DebugLine(bottombackright, bottomfrontright, colour);
				Draw::DebugLine(bottomfrontright, bottomfrontleft, colour);
				Draw::DebugLine(bottomfrontleft, bottombackleft, colour);

				//connect corners vertically
				Draw::DebugLine(bottombackleft, topbackleft, colour);
				Draw::DebugLine(bottombackright, topbackright, colour);
				Draw::DebugLine(bottomfrontright, topfrontright, colour);
				Draw::DebugLine(bottomfrontleft, topfrontleft, colour);
			}

			void Update(Vector^ pos)
			{
				centre = pos;
			}

			void Draw(Colour^ colour)
			{
				float leftX = centre->x - extents->x;
				float bottomY = centre->y - extents->y;
				float backZ = centre->z - extents->z;

				float rightX = centre->x + extents->x;
				float frontZ = centre->z + extents->z;
				float topY = centre->y + extents->y;

				//top face
				Vector^ topbackleft = gcnew Vector(leftX, topY, backZ);
				Vector^ topbackright = gcnew Vector(rightX, topY, backZ);
				Vector^ topfrontright = gcnew Vector(rightX, topY, frontZ);
				Vector^ topfrontleft = gcnew Vector(leftX, topY, frontZ);
				//bottom face
				Vector^ bottombackleft = gcnew Vector(leftX, bottomY, backZ);
				Vector^ bottombackright = gcnew Vector(rightX, bottomY, backZ);
				Vector^ bottomfrontright = gcnew Vector(rightX, bottomY, frontZ);
				Vector^ bottomfrontleft = gcnew Vector(leftX, bottomY, frontZ);
				//draw top face
				Draw::DebugLine(topbackleft, topbackright, colour);
				Draw::DebugLine(topbackright, topfrontright, colour);
				Draw::DebugLine(topfrontright, topfrontleft, colour);
				Draw::DebugLine(topfrontleft, topbackleft, colour);
				//draw bottom fae
				Draw::DebugLine(bottombackleft, bottombackright, colour);
				Draw::DebugLine(bottombackright, bottomfrontright, colour);
				Draw::DebugLine(bottomfrontright, bottomfrontleft, colour);
				Draw::DebugLine(bottomfrontleft, bottombackleft, colour);
				//connect corners vertically
				Draw::DebugLine(bottombackleft, topbackleft, colour);
				Draw::DebugLine(bottombackright, topbackright, colour);
				Draw::DebugLine(bottomfrontright, topfrontright, colour);
				Draw::DebugLine(bottomfrontleft, topfrontleft, colour);
			}

			AABB(Vector^ Centre, Vector^ Extents)
			{
				centre = Centre;
				extents = Extents;
			}

			Vector^ GetMin()
			{
				return gcnew Vector(centre->x - extents->x, centre->y - extents->y, centre->z - extents->z);
			}

			Vector^ GetMax()
			{
				return gcnew Vector(centre->x + extents->x, centre->y + extents->y, centre->z + extents->z);
			}

			Vector^ GetSize()
			{
				return gcnew Vector(extents->x * 2, extents->y * 2, extents->z * 2);
			}

			AABB^ minkowskiDifference(AABB^ other)
			{
				Vector^ topLeft = GetMin() - other->GetMax();
				Vector^ fullSize = GetSize() + other->GetSize();
				return gcnew AABB(topLeft + (fullSize / 2), fullSize / 2);
			}

			static AABB^ Build(List<Vertex^>^ points)
			{
				float maxnum = -FLT_MAX;
				float minnum = FLT_MAX;

				Vector^ min = gcnew Vector(minnum);
				Vector^ max = gcnew Vector(maxnum);

				for each(Vector^ v in points)
				{
					min = Vector::Min(v, min);
					max = Vector::Max(v, max);
				}

				return gcnew AABB((min + max)* 0.5f, (max - min)*0.5f);
			}

			static AABB^ Build(List<Vector^>^ points)
			{
				float maxnum = -FLT_MAX;
				float minnum = FLT_MAX;

				Vector^ min = gcnew Vector(minnum);
				Vector^ max = gcnew Vector(maxnum);

				for each(Vector^ v in points)
				{
					min = Vector::Min(v, min);
					max = Vector::Max(v, max);
				}

				return gcnew AABB((min + max)* 0.5f, (max - min)*0.5f);
			}

			Vector^ ClosestPoint(Vector^ point)
			{
				Vector^ min = GetMin();
				Vector^ max = GetMax();
				float minDist = Math::Abs(point->x - min->x);
				Vector^ boundsPoint = gcnew Vector(min->x, point->y, 0);

				if (Math::Abs(max->x - point->x) < minDist)
				{
					minDist = Math::Abs(max->x - point->x);
					boundsPoint = gcnew Vector(max->x, point->y, 0);
				}

				if (Math::Abs(max->y - point->y) < minDist)
				{
					minDist = Math::Abs(max->y - point->y);
					boundsPoint = gcnew Vector(point->x, max->y, 0);
				}

				if (Math::Abs(min->y - point->y) < minDist)
				{
					boundsPoint = gcnew Vector(point->x, min->y, 0);
				}

				return boundsPoint;
			}

			//https://studiofreya.com/3d-math-and-physics/simple-aabb-vs-aabb-collision-detection/
			bool Overlapping(AABB^ other)
			{
				if (fabs(this->centre->x - other->centre->x) > (this->extents->x + other->extents->x)) return false;
				if (fabs(this->centre->y - other->centre->y) > (this->extents->y + other->extents->y)) return false;
				if (fabs(this->centre->z - other->centre->z) > (this->extents->z + other->extents->z)) return false;
				// We have an overlap
				return true;
			}

			bool Colliding(AABB^ other)
			{
				AABB^ m = minkowskiDifference(other);
				Vector^ min = m->GetMin();
				Vector^ max = m->GetMax();
				if (min->x <= 0 && max->x >= 0 && min->y <= 0 && max->y >= 0 && min->z <= 0 && max->z >= 0)
				{
					return true;
				}
				return false;
			}
		};
	}
}

