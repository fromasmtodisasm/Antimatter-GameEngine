#pragma once
#include "Physics2d.h"
#include "Contact2d.h"
#include "GJK2D.h"
#include "Edge.h"

//contact clipping 3d
//https://stackoverflow.com/questions/16389217/finding-the-intersection-of-two-3d-polygons

namespace Antimatter {
	namespace Games {

		public ref class Manifold2d
		{
		public:
			Manifold2d(Physics2d^ A,Physics2d^ B);
			void Initialize();
			bool IsPair(Physics2d ^ A, Physics2d ^ B);
			bool IsPair(Manifold2d ^ m);
			bool FindContact();
			List<Vector^>^ Clip(Edge ^ edge, Vector ^ normal, float o);
			void GetClippingPoints(Contact2d^ c);
			void Solve();
			List<Contact2d^>^ contacts = gcnew List<Contact2d^>();
			Contact2d^ deepestContact;
			Physics2d^ a;
			Physics2d^ b;
			Vector^ normal;
			GJK2D^ gjk;
			Edge^ refEdge;
			Edge^ incEdge;
			int contactCount;
			int restCounter;
			float penetration;
			float mixedRestitution;
			float mixedDynamicFriction;
			float mixedStaticFriction;
			float restThreshold;
			bool sleep;
			bool debug;
			bool impact;
			bool touching;
			bool resting;
			bool scraping;
			bool alreadyChekced;
			static bool DebugManifold = false;
		};
	}
}
