#pragma once
#include "Manifold.h"


namespace Antimatter {
	namespace Games {

		public ref class PhysicsSolver3D
		{
		public:
			PhysicsSolver3D();
			Manifold^GetManifold(Mesh^ A, Mesh^ B);
			bool Running();
			Physics^ AddThing(Mesh^ mesh);
			Manifold^ FindManifold(Mesh^ a, Mesh^ b);
			List<Manifold^>^ FindManifolds(Mesh ^ a);
			void Step();
			void Debug();
			void DebugPair(Mesh^ m1, Mesh^ m2);
			List<Manifold^>^ manifolds = gcnew List<Manifold^>();
			List<Manifold^>^ narrowPhase = gcnew List<Manifold^>();
			List<Mesh^>^ bodies = gcnew List<Mesh^>();
			bool pause;
			bool step;
		};
	}
}

