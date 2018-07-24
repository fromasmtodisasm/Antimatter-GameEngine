#pragma once
#include "Vertex.h"


namespace Antimatter {
	namespace Games {
		public ref class Edge
		{
		public:
			Edge()
			{
				Vertex^ a = gcnew Vertex();
				Vertex^ b = gcnew Vertex();
				Vertex^ max = gcnew Vertex();
			};

			Edge(Vertex^ A, Vertex^ B)
			{
				a = A;
				b = B;
			};

			Edge(Vector^ A, Vector^ B)
			{
				a = gcnew Vertex(A->x,A->y,A->z);
				b = gcnew Vertex(B->x, B->y, B->z);
			};

			Vertex^ a;
			Vertex^ b;
			Vertex^ max;

			Vector^ edgeVector()
			{
				return b - a;
			}

			float dot2d(Vector^ v)
			{
				return (b - a)->dot2d(v);
			}

			void Draw(Colour^ col, Colour^ col2)
			{
				Draw::DebugLine(a, b, col, col2);
			}
		};
	}
}

