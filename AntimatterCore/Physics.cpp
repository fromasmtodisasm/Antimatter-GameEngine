#include "Physics.h"

using namespace Antimatter::Games;

//time corrected verlet
//http://lonesock.net/article/verlet.html

//2d physics
//http://myselph.de/gamePhysics/unconstrained.html

Physics::Physics(Shape^ s,Transform^ t)
{
	velocity = gcnew Velocity();
	totalForce = Vector::Zero;
	totalTorque = Vector::Zero;
	avgAcceleration = Vector::Zero;
	gravity = Gravity;
	transform = t;
	shape = s;
	density = 5.f;
	SetMassProperties(density);
	resititution = 0.5f;
	friction = 0.5f;
	staticFriction = 0.5f;
	dynamicFriction = 0.5f;
	SetLinearDampening(0.5f);
	SetAngularDampening(0.5f);
	aabb = gcnew AABB(shape,transform->Scale);
	worldRotation = transform->Rotation;
	worldPosition = transform->Position;
	collidable = true;
}

void Physics::UpdateVelocities()
{
	if (!IsMovable())
	{
		velocity->linear = Vector::Zero;
		velocity->angular = Vector::Zero;
		return;
	}

	Vector^ accel = totalForce*(inverseMass* time_step);
	//clear forces for the enxt frame frame
	totalForce = Vector::Zero;
	//store this frame velocity
	Vector^ currentVel = velocity->linear;
	//velocity for the next frame
	velocity->linear = currentVel + accel;

	if (usedampening)
		ApplyLinearDamping();

	//worldPosition = transform->Position + (currentVel + velocity->linear) * 0.5f * time_step;
	worldPosition = transform->Position + velocity->linear*time_step;
	worldRotation = transform->Rotation;
	Vector^ angularAccel = momentInertiaInverse*totalTorque*time_step;
	totalTorque = Vector::Zero;
	velocity->angular += angularAccel;

	if (usedampening)
		ApplyAngularDamping();

	worldRotation += gcnew Quaternion((velocity->angular*time_step*0.5f),0.f)*worldRotation;
	worldRotation = worldRotation->Normalize();
	return;
}

void Antimatter::Games::Physics::UpdateTransform()
{
	if (IsMovable())
	{
		transform->Position = worldPosition;
		transform->Rotation = worldRotation;
	}
	else {
		velocity->linear = Vector::Zero;
		velocity->angular = Vector::Zero;
	}
}

//https://geometrictools.com/Documentation/PolyhedralMassProperties.pdf
void Antimatter::Games::Physics::SetMassProperties(float value)
{
	if (value != 0.f)
	{
		ComputeMassProperties(value);
		localInertiaInverse = &glm::inverse(*localInertia);
		//mass = value*shape->volume;
		//inversemass = 1.f / mass;
		//density = value;
		//localInertia = &ComputeInertiaTensor(mass);
	}else{
		mass = 0.f;
		inverseMass = 0.f;
		isStatic = true;
		density = 0.f;
		localInertia = &glm::mat3(0.f);
		localInertiaInverse = localInertia;
	}
}

void Antimatter::Games::Physics::ApplyImpulse(Vector ^ point, Vector ^ impulse)
{
	if (isStatic)
		return;
	velocity->linear += impulse*inverseMass;
	velocity->angular += momentInertiaInverse*Vector::Cross(point - worldPosition, impulse);
}

void Antimatter::Games::Physics::ApplyMomentum(Vector ^ point, Vector ^ impulse)
{
	if (isStatic)
		return;
	velocity->linear = impulse*inverseMass;
	velocity->angular = momentInertiaInverse*Vector::Cross(point - transform->Position, impulse);
}

glm::mat3 Antimatter::Games::Physics::ComputeInertiaTensor(float d)
{
	//GetBounds();
	Transform^ t = gcnew Transform();
	Vector^ min_b = minBounds;
	Vector^ max_b = maxBounds;
	t->Scale = transform->Scale;
	shape->ComputeBounds(t, min_b, max_b);
	float factor = (1.0f / 3.0f) * d;
	Vector^ realExtent = 0.5f * (max_b - min_b);
	assert(realExtent->x > 0.f && realExtent->y > 0.f && realExtent->z > 0.f);
	float xSquare = realExtent->x * realExtent->x;
	float ySquare = realExtent->y * realExtent->y;
	float zSquare = realExtent->z * realExtent->z;
	glm::mat3 mat = glm::mat3{
		factor * (ySquare + zSquare), 0.0f, 0.0f,
			0.0f, factor * (xSquare + zSquare), 0.0f,
			0.0f, 0.0f, factor * (xSquare + ySquare)
	};
	return mat;
}

void Antimatter::Games::Physics::GetBounds()
{
	shape->ComputeBounds(transform, minBounds, maxBounds);
}

void Antimatter::Games::Physics::DrawBounds(Colour^ col,bool updateBounds)
{
	if(updateBounds)
	GetBounds();
	shape->DrawBounds(maxBounds, minBounds, col);
}

void Antimatter::Games::Physics::ApplyForces()
{
	totalForce += gravity*mass;
}
