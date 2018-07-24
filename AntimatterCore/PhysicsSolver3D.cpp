#include "PhysicsSolver3D.h"
#include "MPR.h"

using namespace Antimatter::Games;


PhysicsSolver3D::PhysicsSolver3D()
{
}

Manifold^ Antimatter::Games::PhysicsSolver3D::GetManifold(Mesh^ A, Mesh^ B)
{
	for each(Manifold^ m in manifolds)
	{
		if (m->IsPair(A->physics, B->physics))
			return m;
	}
	Manifold^ mani = gcnew Manifold(A, B);
	manifolds->Add(mani);
	return mani;
	// TODO: insert return statement here
}

bool Antimatter::Games::PhysicsSolver3D::Running()
{
	if (pause && !step)
		return false;
	if (step)
		return true;
	if (!pause)
		return true;

	return false;
}

Physics ^ Antimatter::Games::PhysicsSolver3D::AddThing(Mesh ^ mesh)
{
	if (mesh->physics == nullptr)
	{
		mesh->physics = gcnew Physics(mesh->shape, mesh);
	}

	if (!bodies->Contains(mesh))
	{
		bodies->Add(mesh);
	}
	return mesh->physics;
}

Manifold ^ Antimatter::Games::PhysicsSolver3D::FindManifold(Mesh^ a, Mesh^ b)
{
	for each(Manifold^ m in manifolds)
	{
		if (m->IsPair(a->physics, b->physics))
			return m;
	}
	return nullptr;
}

List<Manifold^>^ Antimatter::Games::PhysicsSolver3D::FindManifolds(Mesh^ a)
{
	List<Manifold^>^ ms = gcnew List<Manifold^>();
	for each(Manifold^ m in ms)
	{
		if (a == m->A || a == m->B)
			ms->Add(m);
	}
	return ms;
}

void Antimatter::Games::PhysicsSolver3D::Step()
{
	if (!Physics::Break || step)
	{
		// Integrate forces
		for (int i = 0; i < bodies->Count; ++i)
		{
			bodies[i]->physics->ApplyForces();
			bodies[i]->physics->UpdateVelocities();
		}
		
		for (int i = 0; i < bodies->Count; ++i)
		{
			Mesh^ A = bodies[i];
			if (!A->physics->collidable)
				continue;
			for (int j = 0; j < bodies->Count; ++j)
			{
				Mesh^ B = bodies[j];
				if (!B->physics->collidable)
					continue;
				if (A == B)
					continue;
				//if (A->sleep && B->sleep)
				//	continue;
				Manifold^ m = GetManifold(A, B);
				if (!m->IsBroadphaseOverlapping())
				{
					m->inNarrowphase = false;
					narrowPhase->Remove(m);
					m->contacts->Clear();
					m = nullptr;
					continue;
				}else{
					if (!m->inNarrowphase)
					{
						m->inNarrowphase = true;
						narrowPhase->Add(m);
					}
				}
			}
		}

		for (int i = 0; i < narrowPhase->Count; i++)
		{
			Manifold^ m = narrowPhase[i];
			m->colliding = false;
			Contact^ c = m->mpr->Test(m->A, m->B);
			m->contacts->Clear();
			if (c != nullptr)
			{
				m->colliding = true;
				m->Update(c);
				continue;
			}
			else {
				m->deepContact = nullptr;
				m->contacts->Clear();
			}
		}
		
		// Initialize collision
		for (int i = 0; i < manifolds->Count; ++i)
		{
			//if(manifolds[i]->colliding)
				manifolds[i]->PreSolve();
		}

		// Solve collisions
		int m_iterations = 20;
		for (int j = 0; j < m_iterations; ++j)
		{
			for (int i = 0; i < manifolds->Count; ++i)
			{
				manifolds[i]->Solve();
			}
		}
		step = false;

		// Correct positions
		for (int i = 0; i < manifolds->Count; ++i)
		{
			if (manifolds[i]->deepContact == nullptr)
				continue;

			Contact^ c = manifolds[i]->deepContact;
			Contact::ApplyPositionCorrection(c);
			manifolds[i]->deepContact = nullptr;
			manifolds[i]->contacts->Clear();
		}
		
		// Integrate velocities
		for (int i = 0; i < bodies->Count; ++i)
		{
			bodies[i]->physics->UpdateVelocities();
			bodies[i]->physics->UpdateTransform();
		}
	}
}

void Antimatter::Games::PhysicsSolver3D::Debug()
{
	List<Manifold^>^ folds = gcnew List<Manifold^>();
	for each(Mesh^ m in bodies)
	{
		if (m->physics->debug)
		{
			List<Manifold^>^ manifolds = FindManifolds(m);
			for each(Manifold^ dm in manifolds)
			{
				if(!folds->Contains(dm))
				folds->Add(dm);
			}
		}
	}
	for each(Manifold^ m in folds)
	{
		m->Debug();
	}
}

void Antimatter::Games::PhysicsSolver3D::DebugPair(Mesh ^ m1, Mesh ^ m2)
{
	Manifold^ manifold = FindManifold(m1, m2);
	if (manifold != nullptr)
		manifold->Debug();
}
