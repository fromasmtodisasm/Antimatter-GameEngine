#include "Physics2d.h"

using namespace Antimatter::Games;

void Antimatter::Games::Physics2d::Construction()
{
	// Linear components
	position = gcnew Vector();
	velocity = gcnew Vector();
	gravity = gcnew Vector(0.f, -0.75f, 0.f);
	acceleration = 0.f;
	force = Vector::Zero;
	orientation = 0.f;
	angularVelocity = 0.f;
	torque = 0.f;
	density = 1.f;
	dynamicFriction = 0.5f;
	staticFriction = 0.5f;
	kinematic = false;
	restitution = 1.f;
	bounce = 5.f;
	collidable = true;
}

Physics2d::Physics2d(Collider2d^ s)
{
	shape = s->shape;
	Construction();
	position = s->transform->position;
	orientation = s->transform->rotation;
	transform = s->transform;
	shape->ComputeMassProperties();
	SetMassProperties(density);
	collider = s;
}

void Antimatter::Games::Physics2d::SetMassProperties(float Density)
{
	if (Density == 0.f)
	{
		density = 0.f;
		mass = 0.f;
		inverseMass = 0.f;
		inertiaTensor = 0.f;
		inverseInertiaTensor = 0.f;
		isStatic = true;
	}
	else {
		density = Density;
		mass = shape->area*density;
		inverseMass = 1.f / mass;
		inertiaTensor = shape->inertiaTensor*density;
		inverseInertiaTensor = 1.f / inertiaTensor;
		momentOfInertia = inertiaTensor;
		isStatic = false;
	}
}

void Antimatter::Games::Physics2d::ApplyImpulse(Vector^ impulse, Vector^ contactVector)
{
	if (this->kinematic ||this->isStatic)
		return;
	velocity += impulse * inverseMass;
	angularVelocity += inverseInertiaTensor * Vector::Cross2D(contactVector, impulse);
	sleep = false;
}

void Antimatter::Games::Physics2d::UpdateForces()
{
	force += gravity*mass;
}

void Antimatter::Games::Physics2d::ReverseGravity()
{
	force -= gravity;
}


void Antimatter::Games::Physics2d::IntegrateVelocities()
{
	velocity += force * inverseMass * time_step;
	angularVelocity += torque * inverseInertiaTensor * time_step;
	position += velocity * time_step;
	orientation += angularVelocity * time_step;
	force = Vector::Zero;
	torque = 0.f;
	//ApplyDampening();
}

void Antimatter::Games::Physics2d::IntegrateTransform()
{
	transform->position = position;
	transform->rotation = orientation;
}

void Antimatter::Games::Physics2d::ApplyDampening()
{
	float damp = 0.5f;
	velocity *= 1.0f / (1.0f + time_step * damp);
	angularVelocity *= 1.0f / (1.0f + time_step * damp);
}
