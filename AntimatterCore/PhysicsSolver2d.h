#pragma once
#include "GJK2D.h"
#include "Manifold2d.h"

namespace Antimatter {
	namespace Games {

		public ref class PhysicsSolver2d : public SubScene
		{
		public:
			PhysicsSolver2d();
			Manifold2d ^ GetManifold(Physics2d ^ A, Physics2d ^ B);
			bool IsRunning();
			Manifold2d ^ FindManifold(Physics2d ^ a, Physics2d ^ b);
			void Step();
			List<Manifold2d^>^ manifolds = gcnew List<Manifold2d^>();
			List<Physics2d^>^ bodies = gcnew List<Physics2d^>();
		};
	}
}

