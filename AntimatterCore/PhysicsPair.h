#pragma once
#include "Physics.h"

namespace Antimatter {
	namespace Games {
		public ref class PhysicsPair : List<Physics^>
		{
		public:
			PhysicsPair() : List<Physics^>(2)
			{

			}

			PhysicsPair(Physics^ A, Physics^ B) : List<Physics^>(2)
			{
				SetPair(A, B);
			}

			bool IsPair(Physics^ p1, Physics^ p2)
			{
				if (this[0] == p1 && this[1] == p2)
					return true;
				if (this[0] == p2 && this[1] == p1)
					return true;
				return false;
			}

			property Physics^ a
			{
				Physics^ get()
				{
					return this[0];
				};
			}

			property Physics^ b
			{
				Physics^ get()
				{
					return this[1];
				};
			}

			void SetPair(Physics^ A, Physics^ B)
			{
				this->Clear();
				this->Insert(0, A);
				this->Insert(1, B);
			}
			void SetPair(PhysicsPair^ pp)
			{
				this->Clear();
				this->Insert(0, pp[0]);
				this->Insert(1, pp[1]);
			}

			float GetMixedRestitution()
			{
				return Math::Min(a->resititution, b->resititution);
			}

			float GetMixedFrictionDynamic()
			{
				return (a->staticFriction + b->staticFriction)*0.5f;
			}

			float GetMixedFrictionStatic()
			{
				return (a->dynamicFriction + b->dynamicFriction)*0.5f;
			}

			void CancelGravity()
			{
				this[0]->velocity->linear -= this[0]->gravity*this[0]->inverseMass*time_step;
				this[1]->velocity->linear -= this[1]->gravity*this[1]->inverseMass*time_step;
			}

			void CancelVelocity()
			{
				this[0]->CancelLinearVelocity();
				this[1]->CancelLinearVelocity();
			}

			void Freeze(bool value)
			{
				this[0]->frozen = value;
				this[1]->frozen = value;
			}

			float Mass()
			{
				return this[0]->mass + this[1]->mass;
			}

			float InverseMass()
			{
				return this[0]->inverseMass + this[1]->inverseMass;
			}

			bool IsDebuggable()
			{
				if (this[0]->debug || this[1]->debug)
				{
					return true;
				}
				return false;
			}
			bool debugOverlap = true;

			bool AABBOverlap()
			{
				this[0]->aabb->Update(this[0]->transform->Position);
				this[1]->aabb->Update(this[1]->transform->Position);
				return this[0]->aabb->Overlapping(this[1]->aabb);
			}

			bool Overlapping()
			{
				//is there a more efficient?  
				Vector^ target1 = this[1]->worldPosition - this[0]->worldPosition;
				Vector^ f1 = this[0]->shape->GetFarthestPointInDirection(this[0]->transform, -target1);
				Vector^ f2 = this[1]->shape->GetFarthestPointInDirection(this[1]->transform, target1);

				float dist = Vector::DistanceSquared(this[0]->worldPosition, this[1]->worldPosition);
				float r1 = (f1 - this[0]->worldPosition)->m2;
				float r2 = (f2 - this[1]->worldPosition)->m2;
				if (dist <= r1 + r2)
				{
					return true;
				}
				return false;
			}

			void ApplyAngularImpulse(float j, Vector^ p1, Vector^ p2, Vector^ normal, bool flip)
			{
				float direction = 1;
				if (flip)
					direction = -1;
				if (this[0]->IsMovable())
					this[0]->velocity->angular -= this[0]->inverseLocalInertiaMatrix*Vector::Cross(p1, normal * j);
				if (this[1]->IsMovable())
					this[1]->velocity->angular += this[1]->inverseLocalInertiaMatrix*Vector::Cross(p2, normal * j);
			}

			void ApplyMomentum(Vector^ N, float J, Vector^ point)
			{
				if (this[0]->IsMovable())
					this[0]->velocity->linear = N*J*this[0]->inverseMass;
				if (this[1]->IsMovable())
					this[1]->velocity->linear = -N*J*this[1]->inverseMass;

				if (this[0]->IsMovable())
					this[0]->velocity->angular = this[0]->inverseLocalInertiaMatrix*Vector::Cross(point - this[0]->worldPosition, N * J);
				if (this[1]->IsMovable())
					this[1]->velocity->angular = this[1]->inverseLocalInertiaMatrix*Vector::Cross(point - this[1]->worldPosition, N * J);
			}

			void ReverseVelocities()
			{
				this[0]->ReverseVelocity();
				this[1]->ReverseVelocity();
			}

			void ApplyImpulse(Vector^ N, float J, Vector^ point)
			{
				if (this[0]->IsMovable())
					this[0]->velocity->linear += N*J*this[0]->inverseMass;
				if (this[1]->IsMovable())
					this[1]->velocity->linear += -N*J*this[1]->inverseMass;

				if (this[0]->IsMovable())
					this[0]->velocity->angular += this[0]->inverseLocalInertiaMatrix*Vector::Cross(point - this[0]->worldPosition, N * J);
				if (this[1]->IsMovable())
					this[1]->velocity->angular += this[1]->inverseLocalInertiaMatrix*Vector::Cross(point - this[1]->worldPosition, N * J);
			}

			void ApplyLinear(Vector^ v, bool flip)
			{
				float direction = 1;
				if (flip)
					direction = -1;
				if (this[0]->IsMovable())
					this[0]->velocity->linear -= v*this[0]->inverseMass;
				if (this[1]->IsMovable())
					this[1]->velocity->linear += v*this[1]->inverseMass;
			}

			String^ ToString() override
			{
				return String::Concat(a->transform->name, " + ", b->transform->name);
			}

			String^ GetString()
			{
				return ToString();
			}

			void ApplyVelocity(Vector^ v, bool flip)
			{
				float direction = 1;
				if (flip)
					direction = -1;

				if (this[0]->IsMovable())
					this[0]->velocity->linear += -v;
				if (this[1]->IsMovable())
					this[1]->velocity->linear += v;
			}

			void SetVelocities(Vector^ v1, Vector^ w1, Vector v2, Vector^ w2)
			{
				this[0]->velocity->linear = v1;
				this[0]->velocity->angular = w1;

				this[1]->velocity->linear = v2;
				this[1]->velocity->angular = w2;
			}

			void SetVelocities(PhysicsPair^ pair)
			{
				this[0]->velocity->linear = pair->a->velocity->linear;
				this[0]->velocity->angular = pair->a->velocity->angular;

				this[1]->velocity->linear = pair->b->velocity->linear;
				this[1]->velocity->angular = pair->b->velocity->angular;
			}


			float InverseMassK(Vector^ r1, Vector^ r2, Vector^ direction)
			{
				Vector^ r1CrossN = r1->cross(direction);
				Vector^ r2CrossN = r2->cross(direction);

				// Compute the inverse mass matrix K for the penetration constraint
				float m_k = InverseMass() +
					r1CrossN->dot(this[0]->localInertiaInverse * r1CrossN) +
					r2CrossN->dot(this[1]->localInertiaInverse * r2CrossN);

				float imk;
				m_k > 0.0f ? imk = 1.0f / m_k : 0.f;
				return m_k;
			}
		};
	}
}