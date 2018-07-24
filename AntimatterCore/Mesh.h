#pragma once
#include "Thing.h"
#include "Sprite.h"
#include "Shape.h"
#include "Draw.h"
#include "Physics.h"

namespace Antimatter {
	namespace Games {

		public ref class Mesh : public Thing //see Thing base class
		{
		private:
			void Construction()
			{
				isSprite = false; 
				SetColour(Colour::RandomColour());
				graphic = gcnew Graphic();
			}

		public:

			Mesh():Thing()
			{
				Construction();
			}

			Mesh(String^ filePath) : Thing()
			{
				Construction();
				GenerateFromXFile(filePath);
			}

			Shape^ shape;
			Physics^ physics;
			Graphic^ graphic;

			Vertex^ Support(Vector^ direction)
			{
				float furthestDistance  = -FLT_MAX;
				Vertex^ furthestVertex  = nullptr;
				float tol = 0.000008f;
				for(int i=0;i< this->shape->points->Count;i++)
				{
					Vertex^ v = gcnew Vertex(this->gxtransform->staticMatrix*this->shape->points[i],i);
					float distance = Vector::Dot(v, direction);
					if (distance > furthestDistance + tol) 
					{
						furthestDistance = distance;
						furthestVertex = v;
					}
				}

				return furthestVertex;
			}

			bool RayCast(Vector^% origin, Vector^%direction, float% distance, Vector^% point)
			{
				for each (Face^ f in shape->polymesh->faces)
				{
					if (f->Intersect(origin, direction, this, 100000000000.f, distance, point))
						return true;
				}
				return false;
			}

			void Draw() override
			{
				//TODO wrap static renderer with static functions
				if (!visible)
					return;

				Colour^ col = gcnew Colour();
				if (colour != nullptr)
				{
					col = colour;
				}
				else {
					col = defaultColor;
				}
				//if (!debug)
				Draw::Mesh(this, shape->vao, renderSettings, col, texture->id);
				if (debug || (physics != nullptr && physics->debug))
					Draw::TextBillboardInWorld(name, Position + Vector::right*Scale->x, Colour::White, 1.f);
			}

			void DrawFaces(Colour^ col)
			{
				shape->polymesh->DrawFaces(this, col);
			}
			void DrawFaces(List<Face^>^ faceList,Colour^ col)
			{
				for each(Face^ f in faceList)
					f->draw(this, col);
			}

			Vertex^ GetTransformedVertex(int index)
			{
				Vector^ v = this*shape->points[index];
				return gcnew Vertex(v, index);
			}

			List<Vertex^>^ GetTransformedPoints()
			{
				List<Vertex^>^ verts = gcnew List<Vertex^>();
				for each(Vertex^ v in this->shape->points)
				{
					Vertex^ v_new = gcnew Vertex(this*v,v->index);
					verts->Add(v_new);
				}
				return verts;
			}

			List<Vertex^>^ GetScaledPoints()
			{
				List<Vertex^>^ verts = gcnew List<Vertex^>();
				for each(Vertex^ v in this->shape->points)
				{
					Vertex^ v_new = gcnew Vertex(this->Scale*v, v->index);
					verts->Add(v_new);
				}
				return verts;
			}
			
			List<Face^>^ GetSurface(Vector^ direction)
			{
				float maxface = FLT_MAX;
				Vector^ n = Rotation->inverse()*direction; //transform normal to object local space
				Vector^ maxNormal = shape->Faces[0]->faceNormal;
				List<Face^>^ f_ = gcnew List<Face^>();
				Face^ mf = nullptr;
				int maxFaceIndex = 0;
				//we should be able to easily combine these into one loop
				for(int i=0;i<shape->Faces->Count;i++)
				{
					//I guess we need to check the world
					Face^ f = shape->Faces[i];
					float d = f->faceNormal->dot(n);
					if (d < maxface)
					{
						maxface = d;
						maxNormal = f->faceNormal;
						mf = f;
					}
				}
				
				for each(Face^ f in shape->Faces)
				{
					float d = f->faceNormal->dot(n);
					if (d == maxface)
					{
						f_->Add(f);
					}
				}
				return f_;
			}

			List<Face^>^ GetAssociatedFaces(int vertexIndex)
			{
				List<Face^>^ faces = gcnew List<Face^>();
				this->shape->polymesh->Init();

				for each(Face^ f in this->shape->polymesh->faces)
				{
					if (f->A == vertexIndex||f->B == vertexIndex|| f->C == vertexIndex)
						faces->Add(f);
				}
				return faces;
			}

			void DrawNormals(Colour^ col)
			{
				shape->polymesh->DrawNormals(this, col);
			}

			cli::array<Vector^>^ GetCollisionPoints() override
			{
				return GetShapePoints();
			}

			cli::array<Vertex^>^ GetShapePoints()
			{
				cli::array<Vertex^>^ p = gcnew cli::array<Vertex^>(shape->points->Count);
				glm::mat4 modelMatrix = gxtransform->GetWorldMatrix();

				for (int i = 0; i < shape->points->Count; i++)
				{
					glm::vec4  m = modelMatrix* glm::vec4(shape->points[i]->x, shape->points[i]->y, shape->points[i]->z, 1);
					Vertex^ vx = gcnew Vertex();
					vx->x = m.x;
					vx->y = m.y;
					vx->z = m.z;
					vx->index = shape->points[i]->index;
					p[i] = vx;
				}
				return p;
			}

			//generate a mesh from a xfile
			void GenerateFromXFile(String^ s)
			{
				shape = Shape::FromXFile(s);
				name = shape->name;
			}

			static Mesh^ Upcast(Transform^ t)
			{
				return (Mesh^)t;
			}

			static Mesh^ Upcast(Physics^ t)
			{
				return (Mesh^)t->transform;
			}
		};
	}
}

