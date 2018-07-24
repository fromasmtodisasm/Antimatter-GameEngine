#pragma once
#include "Shape.h"
#include "Thing.h"
#include "PhysicsPair.h"
#include "Mesh.h"

/*
//https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physics5collisionmanifolds/2016%20Tutorial%205%20-%20Collision%20Manifolds.pdf
//http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=4&t=288&start=15
//http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?p=&f=&t=226

//persistant vs oneshot
//http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=4&t=1857&start=30
*/

//clamping impulses
//http://allenchou.net/2013/12/game-physics-resolution-contact-constraints/

namespace Antimatter {
	namespace Games {

		public ref struct Contact : PhysicsPair
		{

		public:
			void Construction()
			{
				point1 = gcnew Vector();
				point2 = gcnew Vector();
				closestPoint = gcnew Vector();
				normal = gcnew Vector();
				this->depth = 0.f;
				sumImpulse = 0.f;
				sumFriction = 0.f;
				warmStart = false;
				persistent = false;
				isResting = false;
				debug = false;
				frictionVector1 = gcnew Vector();
				frictionVector2 = gcnew Vector();
				localA = gcnew Vector();
				localB = gcnew Vector();
				globalA = gcnew Vector();
				globalB = gcnew Vector();
				normalA = gcnew Vector();
				normalB = gcnew Vector();
				sumFrictionVector = gcnew Vector();
			}

			Contact() : PhysicsPair()
			{
				Construction();
			}

			Contact(Mesh^ meshA,Mesh^ meshB) : PhysicsPair()
			{
				Construction();
				A = meshA;
				B = meshB;
				SetPair(A->physics, B->physics);
			}

			Mesh^ A;
			Mesh^ B;
			Vector^ point1; // point
			Vector^ point2;
			Vector^ closestPoint;
			Vector^ normal; // normal at point
			Vector^ normalA;
			Vector^ normalB;
			float depth; // penetration depth at point
			float breakDistance;
			Vector^ localA;  //contact point in local coordinates of each object
			Vector^ localB;
			Vector^ globalA;
			Vector^	globalB;
			Vector^ tangent1;
			Vector^ tangent2;
			Vector^ sumFrictionVector;
			Vector^ r1;
			Vector^ r2;
			float constrainedMass;
			float sumImpulse=0.f;
			float sumFriction = 0.f;
			float elasticity_term=0.f;
			float inverseMassK=0.f;
			float appliedImpulse=0.f;
			float distA=0.f;
			float distB=0.f;

			float staticFriction = 0.f;
			float dynamicFriction = 0.f;
			int iteration;
			bool Collided;
			bool isResting;
			bool warmStart;
			bool persistent;
			bool debug;
			bool rotateNormal = false;
			bool invalid = false;
			bool updated = false;
			bool negativeDeltaVelocity = false;

			Vector^ frictionVector1;
			Vector^ frictionVector2;
			Vector^ relativeVelocity;
			
			Vector^ localToGlobalA;
			Vector^	localToGlobalB;

			void PreSolve();
			float SolveSequential();

			static bool ShowFriction;

			static float JacobianEntry(Vector^ jointAxis,
				Quaternion^ world2A,
				Quaternion^ world2B,
				Vector^ inertiaInvA,
				Vector^ inertiaInvB)
			{
				Vector^ m_aJ = world2A*jointAxis;
				Vector^ m_bJ = world2B*-jointAxis;
				Vector^ m_0MinvJt = inertiaInvA * m_aJ;
				Vector^ m_1MinvJt = inertiaInvB * m_bJ;
				float m_Adiag = m_0MinvJt->dot(m_aJ) + m_1MinvJt->dot(m_bJ);
				return m_Adiag;
			}

			static void ApplyPositionCorrection(Contact^ c)
			{
				if (!c->negativeDeltaVelocity)
					return;

				Mesh^ A = c->A;
				Mesh^ B = c->B;

				if (c->depth < -3.1f)
				{
					//A->physics->worldPosition += c->normal*c->depth;
					//A->physics->worldPosition -= c->normal*c->depth;
					//c->point1->DrawAs(c->depth.ToString(), Colour::New, 0.8f);
					//return;
				//	tool::Show("Problem:  depth ", c->depth);
				}

				float percent = 0.8f; // usually 20% to 80%
				float slop = 0.1f; // usually 0.01 to 0.1

				float f = tool::max(-c->depth - slop, 0.0f) / c->InverseMass() * percent;
				
				Vector^ correction = c->normal *f;
				if (c->normal->IsNearZero())
					tool::show("Problem with normal");

				/*
				if (c->a->IsMovable())
					A->physics->worldPosition += correction*A->physics->inverseMass;

				if (c->b->IsMovable())
					  -= correction*B->physics->inverseMass;
					*/
				
				if (c->a->IsMovable())
					A->physics->worldPosition += correction*A->physics->inverseMass;

				if (c->b->IsMovable())
					B->physics->worldPosition -= correction*B->physics->inverseMass;
			}

			static Vector^ ComputeDeltaVelocity(PhysicsPair^ bodies, Vector^ p1, Vector^ p2)
			{
				return bodies[0]->velocity->linear + bodies[0]->velocity->angular->cross(p1) - bodies[1]->velocity->angular - bodies[1]->velocity->angular->cross(p2);
			}

			static Vector^ ComputeDeltaVelocity(Contact^ c)
			{
				Velocity^ v1 = c->a->velocity;
				Velocity^ v2 = c->b->velocity;
				Vector^ va = v1->linear - v1->angular->cross(c->r1);
				Vector^ vb = v2->linear + v2->angular->cross(c->r2);
				return c->relativeVelocity = vb - va;
			}

			Vector^ ComputeFriction(PhysicsPair^ bs, Vector^ r1, Vector^ r2, Vector^ normal,float J)
			{
				Vector^ relativeVelocity = Contact::ComputeDeltaVelocity(bs, r1, r2);
				Vector^ t = Vector::Cross(relativeVelocity, normal%normal);
				//Vector^ tangent = relativeVelocity - Vector::Cross(relativeVelocity, N)*N;

				float tspeed = t->length();
				//tangent = tangent->Normalize();
				if (tspeed > 0)
				{

					//	tangent = tangent;
					float f_static = bs->GetMixedRestitution();
					float f_dynamic = bs->GetMixedFrictionDynamic();
					float jnum = Vector::Dot(-relativeVelocity, t);
					float d_2 = bs->InverseMassK(r1, r2, t);//float fj invmassB + ComputeDenominator(pb, tangent, rb)= tspeed / d_2;  //divide by 0?

					float jt = jnum * d_2;

					float oldSum = 0;
					float maxJT = f_static*sumImpulse;

					float fsumFriction = Math::Min(Math::Max(oldSum, maxJT), -maxJT);
					jt = fsumFriction - oldSum;

					if (jt< -J*f_static || jt> J*f_static)
					{
						jt = jt*f_dynamic;
					}
					//if(enableFriction)
					bs->ApplyLinear(t->normalized()*jt,false);
					bs->ApplyAngularImpulse(jt, r1, r2, t->normalized(), false);
					return t->normalized()*jt;
				}
				return nullptr;
			}

			Vector^ ComputeFriction(float J,float% impulse,Vector^% tang)
			{
				Contact::ComputeDeltaVelocity(this, r1, r2);
				Vector^ t = Vector::Cross(relativeVelocity, normal%normal);
				//Vector^ tangent = relativeVelocity - Vector::Cross(relativeVelocity, N)*N;

				float tspeed = t->length();
				//tangent = tangent->Normalize();
				if (tspeed > 0)
				{

					//	tangent = tangent;
					float f_static = GetMixedRestitution();
					float f_dynamic = GetMixedFrictionDynamic();
					float jnum = Vector::Dot(-relativeVelocity, t);
					float d_2 = InverseMassK(r1, r2, t);//float fj invmassB + ComputeDenominator(pb, tangent, rb)= tspeed / d_2;  //divide by 0?

					float jt = jnum *d_2;
					if (jt< -J*f_static || jt> J*f_static)
					{
						jt = jt*f_dynamic;
					}
					//if(enableFriction)
					//bs->ApplyLinear(t->Normalized()*jt, false);
					//bs->ApplyAngularImpulse(jt, r1, r2, t->Normalized(), false);
					tang = t->normalized();
					impulse = jt;
					return t->normalized()*jt;
				}
				return nullptr;
			}
			
			static Vector^ ComputeFrictionOneshot(PhysicsPair^ bs, Vector^ r1, Vector^ r2, Vector^ normal, float J)
			{
				Vector^ relativeVelocity = Contact::ComputeDeltaVelocity(bs, r1, r2);
				Vector^ t = Vector::Cross(relativeVelocity, normal*normal);
				//Vector^ tangent = relativeVelocity - Vector::Cross(relativeVelocity, N)*N;

				float tspeed = t->length();
				//tangent = tangent->Normalize();
				if (tspeed > 0)
				{

					//	tangent = tangent;
					float f_static = bs->GetMixedRestitution();
					float f_dynamic = bs->GetMixedFrictionDynamic();
					float jnum = Vector::Dot(-relativeVelocity, t);
					float d_2 = bs->InverseMassK(r1, r2, t);//float fj invmassB + ComputeDenominator(pb, tangent, rb)= tspeed / d_2;  //divide by 0?

					float jt = jnum * d_2;
					if (jt< -J*f_static || jt> J*f_static)
					{
						jt = jt*f_dynamic;
					}
					//if(enableFriction)
					bs->ApplyLinear(t->normalized()*jt, false);
					bs->ApplyAngularImpulse(jt, r1, r2, t->normalized(), false);
					return t->normalized()*jt;
				}
				return nullptr;
			}

			
			Contact(Vector^ p, Vector^ n, float d)
			{
				this->point1 = p;
				this->normal = n;
				this->depth = d;
			}

			Contact(Vector^ p, Vector^ n)
			{
				this->point1 = p;
				this->normal = n;
				this->depth = float::PositiveInfinity;
			}

			void Debug()
			{
				//point1->DrawAsPointRaw(Colour::Yellow, 0.1f);
				Vector^ globalA = a->transform*localA;
				Vector^ globalB = b->transform*localB;
				globalA->DrawAsPointRaw(Colour::Yellow, 0.1f);
				globalB->DrawAsPointRaw(Colour::Yellow, 0.1f);

				float distA = (point1 - globalA)->length();
				float distB = (point1 - globalB)->length();
				float dist = (globalA - globalB)->length();

				if (dist > 0.01f)
					invalid = true;
				//tool::Show("Dist A ", distA, "Dist B",distB);
				if (distA > 0.01f || distB > 0.01f)
				{
					invalid = true;
				}
			}

			void DrawDebug(Colour^ col)
			{
				Draw::DebugVector(point1, normal*depth, col);
			}

			void DrawDebug()
			{
				Draw::DebugVector(point1, normal*depth, Colour::New);
			}

		};
	}
}

