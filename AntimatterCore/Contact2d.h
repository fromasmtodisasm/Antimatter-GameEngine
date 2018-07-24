#pragma once

#include "Vector.h"
#include "Physics2d.h"


namespace Antimatter {
	namespace Games {

		public ref class Contact2d
		{
		public:
			Contact2d();

			Physics2d^ a;
			Physics2d^ b;
			Vector^ point1;
			Vector^ point2;
			Vector^ localA;
			Vector^ localB;
			Vector^ normal;
			float depth;
			float sumImpulse;
			float sumTangent;
			float velocityBias;
			bool persistant;
			bool debug;
			float kMass;
			float tMass;
			float mixedRestitution;
			float mixedStaticFriction;
			float mixedDynamicFriction;
			float restThreshold;
			int indexA;
			int indexB;
			void Initialize(Vector^ deltaVelocity,float restitution,Vector^ ra,Vector^ rb);
			void ApplyImpulse();
			void PositionalCorrection();
			static float GlobalRestingThreshold = 2.f;
		};
	}
}
