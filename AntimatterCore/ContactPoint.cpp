#include "SystemHeader.h"
#include "Contact2d.h"
#include <algorithm>

using namespace Antimatter::Games;
//using namespace std;

Contact2d::Contact2d()
{
	restThreshold = GlobalRestingThreshold;
}

void Antimatter::Games::Contact2d::Initialize(Vector^ deltaVel,float mixedRestitution,Vector^ ra,Vector^ rb)
{
	// Calculate static and dynamic friction
	mixedStaticFriction = sqrt(a->staticFriction * b->staticFriction);
	mixedDynamicFriction = sqrt(a->dynamicFriction * b->dynamicFriction);
	sumImpulse = 0.f;
	sumTangent = 0.f;

	if (debug)
	{
		point1->DrawAs("point A", Colour::Green, 0.4f);
	}

	//effective mass of collision
	float raCrossN = Vector::Cross2D(ra, normal);
	float rbCrossN = Vector::Cross2D(rb, normal);
	kMass = a->inverseMass + b->inverseMass + (a->inverseInertiaTensor* (raCrossN*raCrossN) + b->inverseInertiaTensor*(rbCrossN*rbCrossN));

	//effective mass along tangent direction
	//is it ok to precalculate this?

	//tangent vector
	Vector^ t = Vector::Cross2D(normal, 1.f);

	float raCrossT = Vector::Cross2D(ra, t);
	float rbCrossT = Vector::Cross2D(rb, t);

	tMass = a->inverseMass + b->inverseMass + (a->inverseInertiaTensor* (raCrossT*raCrossT) + b->inverseInertiaTensor*(rbCrossT*rbCrossT));

	// Determine if we should perform a resting collision or not
	// The idea is if the only thing moving this object is gravity,
	// then the collision should be performed without any restitution
	//if (rv->lengthSquared2d < (time_step * gravity).LenSqr() + EPSILON)
	//	e = 0.0f;
	
	float velocityThreshold = 0.01f;
	velocityBias = 0.0f;
	float vRel = Vector::Dot2D(normal, b->velocity + Vector::Cross2D(b->angularVelocity, rb) - a->velocity - Vector::Cross2D(a->angularVelocity, ra));
	if (vRel < -velocityThreshold)
	{
		velocityBias = -mixedRestitution * vRel;
	}
}

void Antimatter::Games::Contact2d::ApplyImpulse()
{
	// Early out and positional correct if both objects have infinite mass
	if (a->inverseMass + b->inverseMass == 0.f)
	{
		return;
	}

	Vector^ ra = point1 - a->transform->position;
	Vector^ rb = point1 - b->transform->position;

	// Relative velocity
	Vector^ rv = b->velocity + Vector::Cross2D(b->angularVelocity, rb) -
	a->velocity - Vector::Cross2D(a->angularVelocity, ra);

	// Relative velocity along the normal
	float contactVel = Vector::Dot2D(rv, normal);

	// Calculate impulse scalar
	float j = -(1.0f + mixedRestitution) * contactVel + velocityBias;
	j /= kMass;

	float newImpulse = Math::Max(sumImpulse + j, 0.f);
	j = newImpulse - sumImpulse;
	sumImpulse = newImpulse;

	Vector^ impulse = normal * j;
	a->ApplyImpulse(-impulse, ra);
	b->ApplyImpulse(impulse, rb);

	// Friction impulse
	rv = b->velocity + Vector::Cross2D(b->angularVelocity, rb) -
		a->velocity - Vector::Cross2D(a->angularVelocity, ra);

	Vector^ t = Vector::Cross2D(normal, 1.f);

	// j tangent magnitude
	float jt = -Vector::Dot2D(rv, t);
	jt /= tMass;
	float maxFriction = mixedStaticFriction * sumImpulse;
	float newTangent = tool::clamp(sumTangent + jt, -maxFriction, maxFriction);
	jt = newTangent - sumTangent;
	sumTangent = newTangent;

	Vector^ tangentImpulse = t*jt;
	// Apply friction impulse
	a->ApplyImpulse(-tangentImpulse, ra);
	b->ApplyImpulse(tangentImpulse, rb);

	// Don't apply tiny friction impulses
	//if (jt == 0.0f)
	//	return;
	/*
	// Coulumb's law
	Vector^ tangentImpulse;
	if (std::abs(jt) < j * mixedStaticFriction)
	tangentImpulse = t * jt;
	else
	tangentImpulse = t * -j * mixedDynamicFriction;
	*/
}

void Antimatter::Games::Contact2d::PositionalCorrection()
{
	const float k_slop = 0.01f; // Penetration allowance
	const float percent = 0.2f; // Penetration percentage to correct
	//remember to use the manifold normal
	Vector^ correction = std::max(depth - k_slop, 0.0f) / (a->inverseMass + b->inverseMass) *normal* (depth * percent);
	a->position -= correction*a->inverseMass;
	b->position += correction *b->inverseMass;
}
