#include "PhysicsSolver2d.h"

////https://github.com/RandyGaul/ImpulseEngine/blob/master/Manifold.cpp

using namespace Antimatter::Games;

PhysicsSolver2d::PhysicsSolver2d() : SubScene()
{
}

Manifold2d^ PhysicsSolver2d::GetManifold(Physics2d^ A, Physics2d^ B)
{
	for each(Manifold2d^ m in manifolds)
	{
		if (m->IsPair(A, B))
			return m;
	}
	Manifold2d^ mani = gcnew Manifold2d(A, B);
	manifolds->Add(mani);
	return mani;
}

bool PhysicsSolver2d::IsRunning()
{
	if (pause && !step)
		return false;
	if (step)
		return true;
	if (!pause)
		return true;

	return false;
}

Manifold2d^ PhysicsSolver2d::FindManifold(Physics2d^ a, Physics2d^ b)
{
	for each(Manifold2d^ m in manifolds)
	{
		if (m->IsPair(a, b))
			return m;
	}
	return nullptr;
}

void PhysicsSolver2d::Step()
{
	// Generate new collision info
	for (int i = 0; i < bodies->Count; ++i)
	{
		Physics2d^ A = bodies[i];
		if (!A->collidable)
			continue;
		for (int j = 0; j < bodies->Count; ++j)
		{
			Physics2d^ B = bodies[j];
			if (!B->collidable)
				continue;
			if (A == B)
				continue;
			if (A->sleep && B->sleep)
				continue;
			Manifold2d^ m = GetManifold(A, B);
			//because the loop iterates over all bodies and reverses
			//the order once it gets to B vs A.  So use this bool to prevent
			//duplicate checking.  Need a better way of handling this
			if (m->alreadyChekced)
				continue;

			m->impact = false;
			m->alreadyChekced = true;
			bool contactFound =  m->FindContact();
			
			if (!contactFound)
			{
				m->touching = false;
				m->impact = false;
				manifolds->Remove(m);
				m = nullptr;
				continue;
			}
			else {
				
				if(!m->touching)
					m->impact = true;

				m->touching = true;
				//tool::debug("Touching: " + m->touching, "Impact: " + m->impact);
				continue;
			}
		}
	}

	// Integrate forces
	for (int i = 0; i < bodies->Count; ++i)
	{
		if (bodies[i]->sleep)
			continue;
		bodies[i]->UpdateForces();
		bodies[i]->UpdateVelocities();
	}
	
	// Initialize collision
	for (int i = 0; i < manifolds->Count; ++i)
	{
		manifolds[i]->Initialize();
		manifolds[i]->alreadyChekced = false;
	}

	// Solve collisions
	int m_iterations = 10;
	for (int j = 0; j < m_iterations; ++j)
	{
		
		for (int i = 0; i < manifolds->Count; ++i)
		{
			if (!manifolds[i]->sleep)
			manifolds[i]->Solve();
		}
	}

	// Correct positions
	for (int i = 0; i < manifolds->Count; ++i)
	{
		//if(!manifolds[i]->sleep)
		if(manifolds[i]->deepestContact != nullptr)
			manifolds[i]->deepestContact->PositionalCorrection();
	}


	// Integrate velocities
	for (int i = 0; i < bodies->Count; ++i)
	{
		//bodies[i]->UpdateVelocities();
		if (!bodies[i]->sleep)
			bodies[i]->IntegratePosition();
	}

	for (int i = 0; i < bodies->Count; ++i)
	{
		if(!bodies[i]->sleep)
			bodies[i]->IntegrateTransform();
	}

	// Clear all forces
	/*
	for (int i = 0; i < bodies->Count; ++i)
	{
		Body *b = bodies[i];
		b->force.Set(0, 0);
		b->torque = 0;
	}
	*/
}