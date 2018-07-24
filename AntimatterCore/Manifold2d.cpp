#include "Manifold2d.h"

//https://github.com/RandyGaul/ImpulseEngine/blob/master/Manifold.cpp
using namespace Antimatter::Games;

Antimatter::Games::Manifold2d::Manifold2d(Physics2d ^ A, Physics2d ^ B)
{
	a = A;
	b = B;
	gjk = gcnew GJK2D(A,B);
	debug = DebugManifold;
	alreadyChekced = false;
	impact = false;
	touching = false;
	restThreshold = 2.f;
}

bool Manifold2d::IsPair(Physics2d ^ A, Physics2d ^ B)
{
	if ((a == A && b == B) || (a == B && b == A))
	{
		return true;
	}
	return false;
}

bool Manifold2d::IsPair(Manifold2d^ m)
{
	if ((a == m->a && b == m->b) || (a == m->b && b == m->a))
	{
		return true;
	}
	return false;
}

bool Manifold2d::FindContact()
{
	Contact2d^ cp = gjk->FindContact(a, b);
	//need to remove contacts
	if (cp != nullptr)
	{
		
		GetClippingPoints(cp);
		return true;
	}
	else
		return false;
}

List<Vector^>^ Manifold2d::Clip(Edge^ edge, Vector^ n, float o)
{
	Vector^ v1 = edge->a;
	Vector^ v2 = edge->b;
	List<Vector^>^ points = gcnew List<Vector^>();
	float d1 = n->dot2d(v1) - o;
	float d2 = n->dot2d(v2) - o;

	if (d1 >= 0.0f)
		points->Add(v1);
	if (d2 >= 0.0f)
		points->Add(v2);

	if (d1*d2 < 0.0f)
	{
		Vector^ e = v2 - v1; //get the edgeVector()

		float u = d1 / (d1 - d2);
		e *= u;
		e += v1;
		points->Add(e);
	}

	return points;
}

//sutherland hoffman clipping
//http://www.sirhelpme.com/sutherland-hodgman-polygon-clipping-algorithm/
void Antimatter::Games::Manifold2d::GetClippingPoints(Contact2d^ c)
{
	contacts->Clear();

	if (debug)
	{
		if (!sleep)
		{
			Draw::TextInWorld("A", c->a->position, c->a->collider->colour, 0.5f, 0.5f, true);
			Draw::TextInWorld("B", c->b->position, c->b->collider->colour, 0.5f, 0.5f, true);
		}
		else {
			Draw::TextInWorld("A sleep", c->a->position, c->a->collider->colour, 0.5f, 0.5f, true);
			Draw::TextInWorld("B sleep", c->b->position, c->b->collider->colour, 0.5f, 0.5f, true);
		}
	}

	Edge^ edgeA = c->a->collider->GetClosestEdge(c->indexA, c->normal);
	Edge^ edgeB = c->b->collider->GetClosestEdge(c->indexB, -c->normal);
	refEdge = edgeA;
	incEdge = edgeB;
	bool flip = false;

	if (abs(edgeA->edgeVector()->dot2d(c->normal)) <= abs(edgeB->edgeVector()->dot2d(c->normal)))
	{
		refEdge = edgeA;
		incEdge = edgeB;
	}
	else {
		refEdge = edgeB;
		incEdge = edgeA;
		flip = true;
	}

	if (debug)
	{
		Draw::DebugLine(refEdge->a, refEdge->b, Colour::White);
		Draw::DebugLine(incEdge->a, incEdge->b, Colour::LightBlue);
	}

	Vector^ refV = (refEdge->b - refEdge->a)->normalize2D();

	float o1 = refV->dot2d(refEdge->a);

	List<Vector^>^ clippedPoints = Clip(incEdge, refV, o1);
	if (clippedPoints->Count < 2)
	{
		//tool::Show("only one clip point found.  Abort!  Abort!");
		return;
	}

	float o2 = refV->dot(refEdge->b);
	clippedPoints = Clip(gcnew Edge(clippedPoints[0], clippedPoints[1]), -refV, -o2);
	if (clippedPoints->Count < 2)
	{
		//tool::Show("only one clip point found.  Abort!  Abort!");
		return;
	}

	Vector^ refNorm = gcnew Vector(refV->y, -refV->x);
	//if (flip)
	//	refNorm = -refNorm;

	float max = refNorm->dot2d(refEdge->max);
	//other sources say flip the normal.  flipping max seems to solve our problems though
	if (flip)
		max = -max;
	float depth1 = refNorm->dot2d(clippedPoints[0]) - max;
	float depth2 = refNorm->dot2d(clippedPoints[1]) - max;

	if (debug)
		Draw::DebugVector(refEdge->max, refNorm, Colour::White, "manifold Edge", Colour::White, 0.2f);

	if (depth1 < 0.0f)
	{
		Contact2d^ cp1 = gcnew Contact2d();
		cp1->point1 = clippedPoints[0];
		if (debug)
			cp1->point1->DrawAsPointRaw(Colour::New, 0.1f);
		cp1->normal = refNorm;
		cp1->depth = depth1;
		cp1->a = a;
		cp1->b = b;
		contacts->Add(cp1);
	}

	if (depth2 < 0.0f)
	{
		Contact2d^ cp2 = gcnew Contact2d();
		cp2->point1 = clippedPoints[1];
		if (debug)
			cp2->point1->DrawAsPointRaw(Colour::New, 0.1f);
		cp2->normal = refNorm;
		cp2->depth = depth2;
		cp2->a = a;
		cp2->b = b;
		contacts->Add(cp2);
	}

	if (contacts->Count == 2)
	{
		deepestContact = contacts[0];
		if (contacts[1]->depth < deepestContact->depth)
			deepestContact = contacts[1];
	}
	else if (contacts->Count == 1)
	{
		deepestContact = contacts[0];
	}
}

void Antimatter::Games::Manifold2d::Solve()
{
	for (int i = 0; i < contacts->Count; i++)
		contacts[i]->ApplyImpulse();
}

void Manifold2d::Initialize()
{
	if (contacts->Count < 1)
		return;

	if (restCounter > 50)
	{
		sleep = true;
		a->sleep = true;
		b->sleep = true;
	}
	mixedRestitution = Math::Min(a->restitution, b->restitution);
	Vector^ ra = contacts[0]->point1 - a->position;
	Vector^ rb = contacts[0]->point1 - b->position;
	Vector^ rv = b->velocity + Vector::Cross2D(b->angularVelocity, rb) -
		a->velocity - Vector::Cross2D(a->angularVelocity, ra);

	if (rv->length2D() < restThreshold)
	{
		restCounter++;
		mixedRestitution = 0.f;
	}
	else {
		restCounter = 0;
		sleep = false;
	}
	for (int i = 0; i < contacts->Count; ++i)
	{
		contacts[i]->Initialize(rv,mixedRestitution,ra,rb);
	}
}
