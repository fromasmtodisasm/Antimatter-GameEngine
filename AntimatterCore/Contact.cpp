#include "Contact.h"

using namespace Antimatter::Games;

//https://www.gamedev.net/forums/topic/685359-physics-fix-sinking-in-impulse-calculation/

void Antimatter::Games::Contact::PreSolve()
{
	if (this == nullptr)
		return;
	if (this->a == nullptr || this->b == nullptr)
		return;
	bool disableWarmStart = false;

	sumImpulse = 0.f;
	sumFriction = 0.f;
	iteration = 0;
	//r1 = point1 - A->physics->worldPosition;
	//r2 = point2 - B->physics->worldPosition;

	//Vector^ raCrossN = Vector::Cross(r1, normal);
	//Vector^ rbCrossN = Vector::Cross(r2, normal);

	//float d1 = InverseMass();
	//Vector^ d2 = Vector::Cross(a->localInertiaInverse*raCrossN, r1);
	//Vector^ d3 = Vector::Cross(b->localInertiaInverse*rbCrossN, r2);

	//inverseMassK = d1 + Vector::Dot(normal, d2 + d3);

	Vector^ m_aJ = A->Rotation->inverse()*(r1->cross(normal));
	Vector^ m_bJ = B->Rotation->inverse()*(r2->cross(-normal));
	Vector^ m_0MinvJt = A->physics->localInertiaInverse * m_aJ;
	Vector^ m_1MinvJt = B->physics->localInertiaInverse * m_bJ;
	float m_Adiag = B->physics->inverseMass + m_0MinvJt->dot(m_aJ) + A->physics->inverseMass + m_1MinvJt->dot(m_bJ);

	if (Vector::IsNearZero(m_Adiag))
		inverseMassK = 0.f;
	else
		inverseMassK = 1.f / m_Adiag;

	staticFriction = this->GetMixedRestitution();
	dynamicFriction = this->GetMixedFrictionDynamic();

	Vector^ va = a->velocity->AtPoint(r1);
	Vector^ vb = b->velocity->AtPoint(r2);

	relativeVelocity = vb - va;

	float deltav = -relativeVelocity->dot(normal);
	negativeDeltaVelocity = false;

	if (deltav < 0.f)
		negativeDeltaVelocity = true;

	a->totalForce = Vector::Zero;
	b->totalForce = Vector::Zero;

	
	return;
}

float Contact::SolveSequential()
{
	Vector^ m_aJ = a->worldRotation->inverse()*normal;
	Vector^ m_bJ = b->worldRotation->inverse()*-normal;
	Vector^ va = a->velocity->linear + a->velocity->angular->cross(r1);
	Vector^ vb = b->velocity->linear + b->velocity->angular->cross(r2);
	relativeVelocity = vb - va;

	//ensure the normal point away from A
	/*
	if (normal->dot(point1 - A->Position) > 0.f)
	{
	//	normal = -normal;
		//tool::Show("flip normal");
	}
	*/

	float deltav = relativeVelocity->dot(normal);

	//float r = GetMixedRestitution();

	float bias = 0.8f;
	float slop = 0.1f;
	float correction = (bias / time_step) * fmaxf(-depth - slop, 0.0f);
	
	deltav += correction;

	bool dance = false;
	if (!dance)
		deltav *= inverseMassK;
	
	float newImpulse = fmaxf(sumImpulse + deltav, 0.f);
	float j = newImpulse - sumImpulse;
	sumImpulse = newImpulse;
	
	//j = deltav;
	Vector^ impulse = normal * j;
	if (a->IsMovable())
	{
		glm::mat3 m1 = a->momentInertiaInverse;
		a->velocity->linear += impulse *a->inverseMass;
		a->velocity->angular += m1*Vector::Cross(r1, impulse);
	}

	if (b->IsMovable())
	{
		glm::mat3 m2 = b->momentInertiaInverse;
		b->velocity->linear -= impulse*b->inverseMass;
		b->velocity->angular -= m2*Vector::Cross(r2, impulse);
	}

	// FRICTION
	bool useFriction = true;
	if (useFriction && !impulse->IsNearZero())
	{
		Vector^ va = a->velocity->linear + a->velocity->angular->cross(r1);
		Vector^ vb = b->velocity->linear + b->velocity->angular->cross(r2);
		relativeVelocity = vb - va;
		//if(relativeVelocity->length() > 100.f)
		//	tool::Show("relative velocity", relativeVelocity);
		Vector^ tangent = relativeVelocity - normal*(relativeVelocity ^ normal);
		float tangent_length = tangent->length();
		float cof = GetMixedFrictionStatic();
		if (ShowFriction)
		{
			Draw::DebugLine(point1, point1 + tangent, Colour::New);
			Draw::DebugLine(point1, point1 - tangent, Colour::Red);
		}

		if (tangent_length > 0.01f)
		{
			Vector^ tangentDirection = tangent->normalized();
			Vector^ invMoment1 = a->momentInertiaInverse * (r1 % tangentDirection);
			Vector^ invMoment2 = b->momentInertiaInverse * (r2 % tangentDirection);
			float effectiveMass = InverseMass() + (tangent ^ (invMoment1 % r1 + invMoment2 % r2));
			
			Vector^ tangentVelocityAfter = tangent;

			if (a->IsMovable())
			{
				glm::mat3 m1 = a->momentInertiaInverse;
				a->velocity->linear += tangentVelocityAfter*a->inverseMass;
				a->velocity->angular += m1*Vector::Cross(r1, tangentVelocityAfter);
			}

			if (b->IsMovable())
			{
				glm::mat3 m2 = b->momentInertiaInverse;
				b->velocity->linear -= tangentVelocityAfter*b->inverseMass;
				b->velocity->angular -= m2*Vector::Cross(r2, tangentVelocityAfter);
			}

			return j;

			// Compute effective mass of the constraint system -- this is a measure of how easy it
			// is to accelerate the contact points apart along the constraint direction -- it's analogous
			// to effective resistance in an electric circuit [i.e., 1 / (1/R1 + 1/R2)]
			
			if (effectiveMass > 0.f)
			{
				float mixedFriction = GetMixedFrictionStatic();
				
				float jt = -Vector::Dot(relativeVelocity, tangentDirection)*mixedFriction;
				Vector^ oldFriction = sumFrictionVector;
				
				sumFrictionVector += tangentDirection*jt;
				float t_length = sumFrictionVector->length();

				if (t_length > 0.f && t_length > sumImpulse)
				{
					sumFrictionVector /= t_length*sumImpulse;
				}

				//float tangentImpulse = jt / effectiveMass;
				Vector^ tangentVelocityAfter = (sumFrictionVector-oldFriction)*effectiveMass;
				//tangentVelocityAfter = sumFrictionVector;

				if (a->IsMovable())
				{
					glm::mat3 m1 = a->momentInertiaInverse;
					a->velocity->linear += tangentVelocityAfter *a->inverseMass;
					a->velocity->angular += m1*Vector::Cross(r1, tangentVelocityAfter);
				}

				if (b->IsMovable())
				{
					glm::mat3 m2 = b->momentInertiaInverse;
					b->velocity->linear -= tangentVelocityAfter*b->inverseMass;
					b->velocity->angular -= m2*Vector::Cross(r2, tangentVelocityAfter);
				}
				
			}
		}
	}
	iteration++;
	return j;
}