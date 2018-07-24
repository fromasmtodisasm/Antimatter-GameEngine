#pragma once
#include "Transform2D.h"
#include "Draw.h"
#include "Vertex.h"
#include "Shape2d.h"
#include "Edge.h"

//physia 2d code exmaple
//https://aissabenzayed.wordpress.com/2017/10/30/physia2d-week-9/

namespace Antimatter {
	namespace Games {

		public ref class Collider2d 
		{
		private:
			void Construction()
			{
				colour = gcnew Colour(1.f, 0.f, 0.f, 1.f);
			}

		public:
			Collider2d()
			{
				Construction();
				name = "Collider 2D";
				shape = Shape2D::Box;
			};

			Collider2d(String^ text,Shape2D^ s)
			{
				Construction();
				name = text;
				shape = s;
			};

			Collider2d(String^ text, Shape2D^ s,Colour^ col)
			{
				Construction();
				name = text;
				shape = s;
				colour = col;
			};

			Shape2D^ shape;
			List<Vertex^>^ worldPoints = gcnew List<Vertex^>();
			Transform2D^ transform = gcnew Transform2D();
			Colour^ colour;
			String^ name;

			Vertex^ GetNeighbourVertices(int index, Vertex^% previousVertex, Vertex^% nextVertex)
			{
				//check wworldpoints size first
				int next = index + 1;
				if (next >= worldPoints->Count)
					next = 0;

				nextVertex = worldPoints[next];

				int prev = index - 1;
				if (prev < 0)
					prev = worldPoints->Count-1;

				previousVertex = worldPoints[prev];

				return worldPoints[index];
			}

			void DebugClosestEdge(int index,Vector^ normal)
			{
				Vertex^ aP;
				Vertex^ aN;
				Vertex^ w = GetNeighbourVertices(index, aP, aN);

				if (normal->dot2d(w - aP) < normal->dot2d(w - aN))
					Draw::DebugLine(w, aP, Colour::White);
				else
					Draw::DebugLine(w, aN, Colour::White);
			}

			void GetClosestEdge(int index, Vector^ normal,Vertex^% A,Vertex^% B)
			{
				Vertex^ aPrev;
				Vertex^ aNext;

				A = GetNeighbourVertices(index, aPrev, aNext);

				Vector^ left = (A - aPrev)->normalize2D();
				Vector^ right = (A - aNext)->normalize2D();

				if (right->dot2d(normal) <= left->dot2d(normal))
				{
					B = aNext;
					//B = A;
					//A = aPrev;
				}
				else
				{
					B = A;
					A = aPrev;
					//B = aNext;
				}
			}

			Edge^ GetClosestEdge(int index, Vector^ normal)
			{
				Vertex^ aPrev;
				Vertex^ aNext;
				Edge^ e = gcnew Edge();

				Vertex^ A = GetNeighbourVertices(index, aPrev, aNext);

				Vector^ left = (A - aNext)->normalize2D();
				Vector^ right = (A - aPrev)->normalize2D();
				
				//may not need to use abs here
				if (right->dot2d(normal) <= left->dot2d(normal))
				{
					e->a = aPrev;
					e->b = A;
					e->max = A;
				}
				else
				{
					e->a = A;
					e->b = aNext;
					e->max = aNext;
				}

				return e;
			}

			Vertex^ GetSupportPoint(Vector^ direction)
			{
				int index = 0;
				float maxDot = worldPoints[index]->dot(direction);

				for (int i = 1; i < worldPoints->Count; i++)
				{
					float d = worldPoints[i]->dot(direction);
					if (d > maxDot)
					{
						maxDot = d;
						index = i;
					}
				}

				return gcnew Vertex(worldPoints[index],index);
			}

			void DrawIndex(int index,  Colour^ col, float scale)
			{
				Draw::TextBillboardInWorld(index.ToString(), this->worldPoints[index], col, scale);
			}

			void DrawIndices(Colour^ col,float size)
			{
				for (int i = 0; i < this->worldPoints->Count; i++)
					DrawIndex(i,col, size);
			}

			void DrawDots(Colour^ col, float size)
			{
				for (int i = 0; i < this->worldPoints->Count; i++)
					Draw::TextBillboardInWorld(".", this->worldPoints[i], col, size);
			}

			void Update()
			{
				worldPoints->Clear();
				transform->UpdateMatrix();
				int count = shape->points->Count;
				for (int i = 0; i < count; i++)
				{
					Vector^ p = transform->Transform(shape->points[i]);
					worldPoints->Add(gcnew Vertex(p,i,colour));
				}
			}

			void Debug(float scale)
			{
				Draw::TextBillboardInWorld(name, transform->position, colour, scale);
			}

			void Draw(Colour^ col)
			{
				int n = worldPoints->Count;
				for (int i = 0; i < n; i++)
				{
					int j = i + 1;
					if (j >= n)
						j = 0;

					Draw::DebugLine(worldPoints[i], worldPoints[j], col, col);
				}
			}

			void Draw()
			{
				int n = worldPoints->Count;
				for (int i = 0; i < n; i++)
				{
					int j = i + 1;
					if (j >= n)
						j = 0;

					Draw::DebugLine(worldPoints[i], worldPoints[j], worldPoints[i]->colour, worldPoints[j]->colour);
				}
			}

			void DrawAsCube(Colour^ colour)
			{
				Draw(colour);
				return;
			}

			void DrawAsTriangle(Colour^ colour)
			{
				Draw(colour);
				return;
			}
		};
	}
}

