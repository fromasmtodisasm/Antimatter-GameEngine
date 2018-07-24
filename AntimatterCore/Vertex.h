#pragma once
#include "Vector.h"
#include "glaxRenderer.h"
#include "Draw.h"
#include "Colour.h"

namespace Antimatter {
	namespace Games {

		public ref class Vertex : Vector
		{
		public:


			int index;
			Colour^ colour;
			String^ text;
			Vector^ vertexNormal;

			Vertex() :Vector()
			{
				index = -1;
				text = ". v";
				colour = Colour::White;
			}

			Vertex(float X, float Y, float Z)
			{
				x = X;
				y = Y;
				z = Z;
				index = -1;
				text = ". v";
				colour = Colour::White;
			}

			Vertex(float X, float Y)
			{
				x = X;
				y = Y;
				z = 0.f;
				index = -1;
				text = ". v";
				colour = Colour::White;
			}

			Vertex(float X, float Y,int i)
			{
				x = X;
				y = Y;
				z = 0;
				index = i;
				text = i.ToString();
				colour = Colour::White;
			}

			Vertex(Vector^ v)
			{
				x = v->x;
				y = v->y;
				z = v->z;
			}

			Vertex(Vector^ v,String^ tex)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				text = tex;
			}

			Vertex(Vector^ v,int i)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				index = i;
				text = i.ToString();
				colour = Colour::White;
			}

			Vertex(Vector^ v, int i,Vector^ n)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				index = i;
				text = i.ToString();
				colour = Colour::White;
				vertexNormal = n;
			}

			Vertex(Vector^ v, int i,Colour^ col)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				index = i;
				text = i.ToString();
				colour = col;
			}

			Vertex(Vector^ v, Colour^ col, String^ s)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				colour = col;
				text = s;
			}

			Vertex(Vector^ v, Colour^ col)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				colour = col;
				text = ".";
				index = -1;
			}

			Vertex(Vector^ v,Colour^ col,String^ s,int i)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				colour = col;
				text = s;
				index = i;
			}

			void Load(String^ s, Colour^ col)
			{
				text = s;
				colour = col;
			}

			void Load(String^ s, Colour^ col,int id)
			{
				text = s;
				colour = col;
				index = id;
			}

			void Draw(float scale)
			{
				Draw::TextBillboardInWorld(text, this, colour, scale);
			}

			void Draw()
			{
				Draw::TextBillboardInWorld(text, this, colour, 1.f);
			}

			void Draw(String^ s)
			{
				Draw::TextBillboardInWorld(s, this, colour, 1.f);
			}

			void Draw(Colour^ col)
			{
				Draw::TextBillboardInWorld(text, this, col, 1.f);
			}

			void Draw(String^ tex,Colour^ col)
			{
				Draw::TextBillboardInWorld(tex, this, col, 1.f);
			}

			void DrawLine(Vertex^ vec)
			{
				Draw::TextBillboardInWorld(text, this, colour, 1.f);
				Draw::TextBillboardInWorld(vec->text, vec, vec->colour, 1.f);
				Draw::DebugLine(this, vec, colour, vec->colour);
			}
		};

		public ref class SupportPoint : Vertex
		{
		public:

			Vertex^ vertexA;
			Vertex^ vertexB;

			SupportPoint() :Vertex()
			{
			}

			SupportPoint(float X, float Y, float Z)
			{
				x = X;
				y = Y;
				z = Z;
				w = 1;
			}
	
			SupportPoint(Vector^ v,Vertex^ vA,Vertex^ vB)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				w = 1;
				vertexA = vA;
				vertexB = vB;
			}

			SupportPoint(Vector^ v)
			{
				x = v->x;
				y = v->y;
				z = v->z;
				w = 1;
			}

			void DebugDraw(String^ indexString,Colour^ col,float scale)
			{
				Draw::DebugLine(this, vertexA, col);
				Draw::TextBillboardInWorld(vertexA->index.ToString(), vertexA, col, scale);

				Draw::DebugLine(this, vertexB, col);
				Draw::TextBillboardInWorld(vertexB->index.ToString(), vertexB, col, scale);

				Draw::TextBillboardInWorld(indexString, this, col, scale);
				Draw::DebugPointRaw(this, col, scale*0.5f);
			}

			bool Is(SupportPoint^ sp)
			{
				if ((vertexA->index == sp->vertexA->index || vertexA->index == sp->vertexB->index)
					&& (vertexB->index == sp->vertexA->index || vertexB->index == sp->vertexB->index))
					return true;

				return false;
			}
		};
	}
}

