#pragma once
#include "Velocity.h"
#include "Collider2d.h"

namespace Antimatter {
	namespace Games {

		public ref class Physics2d
		{

		private:
			void Construction();

		public:
			Physics2d(Collider2d^ shape);
			Shape2D^ shape;
			Transform2D^ transform;
			Collider2d^ collider;
			// Linear components
			Vector^ position;
			Vector^ velocity;
			Vector^ gravity;
			Vector^ force;
			float acceleration;
			float mass;
			float inverseMass;
			float density;
			float restitution;
			float staticFriction;
			float dynamicFriction;
			float bounce;
			float orientation; //radians
			float angularVelocity;
			float torque;
			float momentOfInertia;
			float inertiaTensor;
			float inverseInertiaTensor;
			bool kinematic;
			bool isStatic;
			bool collidable;
			bool sleep;

			void UpdateForces();
			void ReverseGravity();
			void IntegrateVelocities();
			void IntegrateTransform();
			void ApplyDampening();
			void SetMassProperties(float density);
			void ApplyImpulse(Vector^ impulse, Vector^ contactVector);

			void UpdateVelocities()
			{
				if (inverseMass == 0.0f)
					return;

				velocity += (force * inverseMass) * (time_step / 2.0f);
				angularVelocity += torque * inverseInertiaTensor * (time_step / 2.0f);
			}
			//why is update velocity time_step/2 but not integrate position?
			void IntegratePosition()
			{
				if (inverseMass == 0.0f)
					return;

				position += velocity * time_step;
				orientation += angularVelocity * time_step;
			}
		};
	}
}

