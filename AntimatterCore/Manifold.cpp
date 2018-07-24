#include "Manifold.h"
using namespace Antimatter::Games;

Manifold::Manifold(Mesh^ m1,Mesh^ m2)
{
	A = m1;
	B = m2;
	Construction();
	this->SetPair(m1->physics, m2->physics);
	debug = false;
	broadPhaseOverlap = false;
	colliding = false;
}

void Antimatter::Games::Manifold::Update(Contact ^ c)
{
	CreateOneShot(c);
}

List<Face^>^ Manifold::GetSurface(Vector^ direction, Mesh^ mesh)
{
	float maxface = -FLT_MAX;
	direction->w = 0.f;
	Vector^ n = mesh->Rotation->inverse()*direction;
	//probably need to transform the normal into correct coordinate space
	Vector^ maxNormal = mesh->shape->Faces[0]->faceNormal;
	List<Face^>^ f_ = gcnew List<Face^>();
	Face^ mf = nullptr;
	int maxFaceIndex = 0;
	//we should be able to easily combine there into one loop
	for (int i = 0; i < mesh->shape->Faces->Count; i++)
	{
		//I guess we need to check the world
		Face^ f = mesh->shape->Faces[i];
		float d = f->faceNormal->dot(n);
		if (d > maxface)
		{
			maxface = d;
			maxNormal = f->faceNormal;
			mf = f;
		}
	}

	for each(Face^ f in mesh->shape->Faces)
	{
		float d = f->faceNormal->dot(n);
		if (d == maxface)
		{
			f_->Add(f);
		}
	}
	return f_;
}

List<Vertex^>^ Manifold::GetClippingPoints(List<Face^>^ faces, Mesh^ mesh)
{
	List<Vertex^>^ points = gcnew List<Vertex^>();
	List<int>^ indices = gcnew List<int>();
	for each(Face^ f in faces)
	{
		Vector^ worldfaceNormal = mesh->Rotation*f->faceNormal->W(0.f);
		Vertex^ a = gcnew Vertex(mesh*mesh->shape->points[f->A], f->A, worldfaceNormal);
		Vertex^ b = gcnew Vertex(mesh*mesh->shape->points[f->B], f->B, worldfaceNormal);
		Vertex^ c = gcnew Vertex(mesh*mesh->shape->points[f->C], f->C, worldfaceNormal);

		if (!indices->Contains(f->A))
		{
			indices->Add(f->A);
			points->Add(a);
		}
		if (!indices->Contains(f->B))
		{
			indices->Add(f->B);
			points->Add(b);
		}
		if (!indices->Contains(f->C))
		{
			indices->Add(f->C);
			points->Add(c);
		}
	}
	return points;
}

List<ClippingEdge^>^ Manifold::GetClippingPlanes(List<Face^>^ faces, Mesh^ mesh)
{
	List<ClippingEdge^>^ edges = gcnew List<ClippingEdge^>();
	List<ClippingEdge^>^ removedEdges = gcnew List<ClippingEdge^>();
	for each(Face^ f in faces)
	{
		Vector^ worldfaceNormal = mesh->Rotation*f->faceNormal->W(0.f);
		Vertex^ a = gcnew Vertex(mesh*mesh->shape->points[f->A], f->A, worldfaceNormal);
		Vertex^ b = gcnew Vertex(mesh*mesh->shape->points[f->B], f->B, worldfaceNormal);
		Vertex^ c = gcnew Vertex(mesh*mesh->shape->points[f->C], f->C, worldfaceNormal);

		//face normal added here is in local coordinates
		ClippingEdge^ eA = gcnew ClippingEdge(a, b, f->A, f->B, f->faceNormal);
		ClippingEdge^ eB = gcnew ClippingEdge(b, c, f->B, f->C, f->faceNormal);
		ClippingEdge^ eC = gcnew ClippingEdge(c, a, f->C, f->A, f->faceNormal);
		eA->worldFaceNormal = worldfaceNormal;
		eB->worldFaceNormal = eA->worldFaceNormal;
		eC->worldFaceNormal = eA->worldFaceNormal;

		eA->mesh = mesh;
		eB->mesh = mesh;
		eC->mesh = mesh;

		bool AddA = true;
		bool AddB = true;
		bool AddC = true;
		for (int i = 0; i < edges->Count; i++)
		{
			if (eA->isReverseEdge(edges[i]))
			{
				//edges->Remove(edges[i]);
				//As an optimisation, we could compute world points are we've determined 
				//the edge is not exterior to the surface.  
				AddA = false;
				removedEdges->Add(edges[i]);
			}

			if (eB->isReverseEdge(edges[i]))
			{
				AddB = false;
				removedEdges->Add(edges[i]);
			}
			if (eC->isReverseEdge(edges[i]))
			{
				AddC = false;
				removedEdges->Add(edges[i]);
			}
		}

		if (AddA)
			edges->Add(eA);

		if (AddB)
			edges->Add(eB);

		if (AddC)
			edges->Add(eC);
	}

	for each(ClippingEdge^ ce in removedEdges)
	{
		edges->Remove(ce);
	}
		
	return edges;
}

//https://gamedev.stackexchange.com/questions/61099/simple-3d-collision-detection-for-general-polyhedra/61107
//gauss maps
//http://www.gdcvault.com/play/1017646/Physics-for-Game-Programmers-The
//https://www.gamedev.net/forums/topic/588070-seperating-axis-theorem---how-to-resolve-contact-points/
//https://hal.archives-ouvertes.fr/hal-01157239/document

//http://www.sunshine2k.de/coding/java/SutherlandHodgman/SutherlandHodgman.html


//c#
//http://rosettacode.org/wiki/Sutherland-Hodgman_polygon_clipping
//may not need this fucntion
bool Manifold::IsClockwise(List<Vertex^>^ polygon)
{
	for (int cntr = 2; cntr < polygon->Count; cntr++)
	{
		bool isLeft = IsOutside(polygon[0], polygon[1], polygon[cntr]);
	//	some of the points may be colinear.  That's ok as long as the overall is a polygon
		return !isLeft;
	}
	return false;
	//throw new ArgumentException("All the points in the polygon are colinear");
}

//http://www.sunshine2k.de/coding/java/SutherlandHodgman/SutherlandHodgman.html
List<Vertex^>^ Manifold::GetIntersectionPolygon(List<Face^>^ clipPoly, Mesh^ inc_mesh, List<Face^>^ refPoly,Mesh^ ref_mesh)
{
	if (clipPoly->Count < 1 || refPoly->Count < 1)
	{
		tool::show("Wanting at least 1 face: ", clipPoly->Count, refPoly);
		return nullptr;
	}

	//get the vertices from the inc mesh for clipping
	List<Vertex^>^ clipPoints = GetClippingPoints(clipPoly,inc_mesh);
	//get the planes form the ref mesh we will clip the points against
	List<ClippingEdge^>^ clipping = GetClippingPlanes(refPoly, ref_mesh);

	//currently does not explicitly cover cases where all the points are within
	List<Vertex^>^ output = gcnew List<Vertex^>();

	//get the centre of the clipping planes.  We will use this
	//to check the direction of the edge and correct the winding 
	//as we iterate 
	Vector^ centre = gcnew Vector();
	for (int i = 0; i < clipping->Count; i++)
	{
		centre += clipping[i]->a;
	}
	centre /= (float)clipping->Count;

	int cpcount = 0;
	//loop through each clipping plane
	//tool::Show(clipPoints->Count);
	for each(ClippingEdge^ cp in clipping)
	{
	
		//check direction the edge if going based on the clipping
		//planes centre.  All normals are imagined to be pointing
		//outwards from the edge, so we can just check the direction
		//of the edge normal
		float co = cp->GetEdgeNormal()->dot(cp->a - centre);
		if (co > 0.f)
		{
			//swap a and b to reverse the edge
			Vector^ tmp = cp->a;
			cp->a = cp->b;
			cp->b = tmp;
		}
		if (debug)
		{
			cpcount++;
			cp->Draw("Cp" + cpcount, Colour::Yellow, Colour::Green,0.1f);
		}
		//clear the list
		//output->Clear();
		for (int i = 0; i < clipPoints->Count; i++)
		{
			if (clipPoints->Count == 0)
				break;

			//get the next point
			//we currently don't enforce a winding
			//direction for the points, so if any instabilities or edge cases,
			//do consider it.
			int j = i + 1;
			if (j >= clipPoints->Count)
				j = 0;

			Vector^ p = clipPoints[i];  //tail point
			Vector^ pn = clipPoints[j]; //head point

			//test which side of the current clipping plane
			//the edge is on
			if (Vector::InsidePlane3d(cp->GetEdgeNormal(), cp->a, p))
			{
				//if the tail is within the clipping plane, add it
				output->Add(gcnew Vertex(p));

				//if the head outside the clipping plabne, then get an
				//intersection point
				if (!Vector::InsidePlane3d(cp->GetEdgeNormal(), cp->a, pn))
				{
					Vector^ newPoint = Vector::RayPlaneIntersection(p, pn, cp->a, cp->GetEdgeNormal());
		
					if (newPoint != nullptr)
					{
						output->Add(gcnew Vertex(newPoint));
					}
				}
				else
				{
					//is the head is also in the clipping plane, add it too
					//output->Add(gcnew Vertex(pn));
				}
			}
			else 
			{  //if the tail is not in the clipping zone
				//check if the head is.  if true then clip the edge and
				//add the head point as well
				if (Vector::InsidePlane3d(cp->GetEdgeNormal(), cp->a, pn))
				{
					Vector^ newPoint = Vector::RayPlaneIntersection(p, pn, cp->a, cp->GetEdgeNormal());
					//cp->a->DrawTo(ray->a, Colour::Purple);
					if (newPoint != nullptr)
					{
						//newPoint->DrawAsPointRaw(Colour::New, 0.1f);
						//Draw::DebugLine(p, pn, Colour::Red, Colour::Orange);
						//p->DrawTo(pn, "tail", Colour::Red, Colour::Orange, 0.4f);
						output->Add(gcnew Vertex(newPoint));
					}
					//pn->DrawAsPointRaw(Colour::New, 0.1f);
					output->Add(gcnew Vertex(pn));
				}
			}
		}
		//copy the vertices to the clip points list so we
		//can iterate over the evolving polygon
		//tool::Show(output->Count);
		clipPoints->Clear();
		for each(Vertex^ v in output)
			clipPoints->Add(v);
		output->Clear();
	}

	if (debug)
	{
		for (int i = 0; i < clipPoints->Count; i++)
		{
			int j = i + 1;
			if (j >= clipPoints->Count)
				j = 0;
			Draw::DebugLine(clipPoints[i], clipPoints[j], Colour::Purple);
			//clipPoints[i]->DrawAsPointRaw(Colour::New, 0.1f);
		}
	}
	
	return clipPoints;
}

//contact clipping stuff
//https://www.gamedev.net/forums/topic/667499-3d-sat-problem/?page=2
//tolerances
//http://realtimecollisiondetection.net/pubs/Tolerances/

//cubeCSharp
//https://github.com/madhawav/Qu3ECsharp/blob/master/QU3ECsharp/Qu3ECSharp/Collision/Collide.cs

//box 2d life js
//https://github.com/natelong/box2d-lite-js/blob/master/src/main/arbiter.js


//filering edge cases
//https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=10603
//http://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physics5collisionmanifolds/2017%20Tutorial%205%20-%20Collision%20Manifolds.pdf
void Manifold::CreateOneShot(Contact^ c)
{
	contact = c;
	Vector^ n = c->normal->n;
	if (c->depth < 0.f)
	{
		c->point1->DrawAs("Depth is negative: " + c->depth.ToString(), Colour::Red);
	}

	contacts->Clear();
	if (Vector::IsNearZero(n->dot(n)))
		tool::show("Error with normal");
	//ensure the normal point away from A
	if (debug)
	{
		A->Position->DrawTo(A->Position + n, Colour::Blue, Colour::LightBlue);
		if (n->dot(c->point1 - A->Position) > 0.f)
		{
			n = -n;
			Draw::DebugLine(A->Position, c->point1, A->colour, Colour::Green);
		}
		else {
			Draw::DebugLine(A->Position, c->point1, A->colour, Colour::White);
		}
	}

	List<Face^>^ edgesA = GetSurface(-n,A);
	List<Face^>^ edgesB = GetSurface(n,B);

	Colour^ refCol = Colour::White;
	Colour^ incCol = Colour::LightBlue;

	List<Face^>^ refEdges = edgesA;
	List<Face^>^ incEdges = edgesB;

	Mesh^ refMesh;
	Mesh^ incMesh;

	bool flip = false;

	if (edgesA->Count == 0 || edgesB->Count == 0)
	{
		Contact^ contact = gcnew Contact(A, B);
		c->point1->DrawAsPointRaw(Colour::New, 0.1f);
		Draw::DebugLine(c->point1, c->point1 + n, Colour::Yellow);
		c->point1->DrawAs("face count 0, possible degenerate case", Colour::Orange);
		contact->normal = c->normal;
		contact->depth = c->depth;
		contact->point1 = c->point1;
		contact->point2 = c->point2;
		contact->r1 = c->point1 - A->Position;
		contact->r2 = c->point2 - B->Position;
		contacts->Add(contact);
		deepContact = contact;
		return;
	}

	if (edgesA->Count == 1 || edgesB->Count == 1)
	{
		if(debug)
		c->point1->DrawAs("Possible corner collision", Colour::New);
		return;
	}

	//flip inc and ref planes based on which is most parallel to the normal
	if ((A->Rotation*edgesA[0]->faceNormal->W(0.f))->dot(n) < (B->Rotation*edgesB[0]->faceNormal->W(0.f))->dot(n))
	{
		refEdges = edgesA;
		incEdges = edgesB;
		refMesh = A;
		incMesh = B;
	}
	else{
		refEdges = edgesB;
		incEdges = edgesA;
		refMesh = B;
		incMesh = A;
		flip = true;
		if(debug)
		c->point1->DrawAs("faces flipped",Colour::New);
	}
	if (debug)
	{
		ClippingEdge::DrawEdges(refEdges, refMesh,refCol);
		ClippingEdge::DrawEdges(incEdges, incMesh,incCol);
	}
	//we need a point on the ref plave to check if the point is above or below the plane
	Vector^ worldRefPoint = refMesh*refMesh->shape->points[refEdges[0]->A];
	Vector^ worldRefNormal = (refMesh->Rotation*refEdges[0]->faceNormal->W(0.f))->normalized(); //may not need to normalise although consider we use this to retrieve depth

	if (debug)
	{
		refMesh->Position->DrawAs("Reference", refCol);
		incMesh->Position->DrawAs("Incident", incCol);
		(A->Position - gcnew Vector(0.f, 0.5f, 0.f))->DrawAs("A", A->colour);
		(B->Position - gcnew Vector(0.f, 0.5f, 0.f))->DrawAs("B", B->colour);
		worldRefPoint->DrawTo(worldRefPoint + n, Colour::Red, Colour::Yellow);
	}
	//get clipped points
	List<Vertex^>^ finalPoints = GetIntersectionPolygon(incEdges, incMesh,refEdges,refMesh);
	if (finalPoints == nullptr)
		return;
	
	for each(Vector^ v in finalPoints)
	{
		float depth = worldRefNormal->dot(v - worldRefPoint);
		//add the point is it's below the plane
		if (depth <= 0.f)
		{
			Contact^ contact = gcnew Contact(A, B);
			contact->point1 = v;
			contact->normal = n;
			contact->depth = depth;
			
			if (flip)
				contact->normal = -n;
			
			contact->point2 = v+n*depth;
			contact->r1 = contact->point1 - A->Position;
			contact->r2 = contact->point2 - B->Position;
			contacts->Add(contact);
		}
	}

	bool reduce = false;
	if (reduce)
	{
		for (int i = 0; i < contacts->Count; i++)
		{
			Vector^ v = contacts[i]->point1;
			for (int j = 0; j < contacts->Count; j++)
			{
				Vector^ vo = contacts[j]->point1;
				if (i == j)
					continue;

				float d = Vector::Distance(vo, v);

				if (d < 0.6f)
				{
					contacts->Remove(contacts[j]);
				}
			}
		}
	}

	//Get the deepest contact
	int deepest = 0;
	float depth = FLT_MAX;

	for (int i = 0; i < contacts->Count; i++)
	{
		if (contacts[i]->depth < depth)
		{
			deepest = i;
			depth = contacts[i]->depth;
		}
	}
	
	if(contacts->Count > 0)
		deepContact = contacts[deepest];
	
	if (debug)
	{
		for each(Contact^ c in contacts)
		{
			c->point1->DrawAsPointRaw(Colour::New, 0.02f);
			Draw::DebugLine(c->point1, c->point2, Colour::Green,Colour::Green->Alpha(0.f));
		}
	}
}

void Antimatter::Games::Manifold::TestClipping()
{
	float X_ext = 1.f;
	float Y_ext = -2.f;
	float Z_ext = 2.f;

	List<ClippingEdge^>^ clipping = gcnew List<ClippingEdge^>();

	ClippingEdge^ cp1 = gcnew ClippingEdge(gcnew Vector(X_ext, Y_ext, Z_ext), gcnew Vector(X_ext, Y_ext, -Z_ext));
	cp1->worldFaceNormal = Vector::down;

	ClippingEdge^ cp2 = gcnew ClippingEdge(gcnew Vector(-X_ext, Y_ext, -Z_ext), gcnew Vector(X_ext, Y_ext, -Z_ext));
	cp2->worldFaceNormal = Vector::down;

	ClippingEdge^ cp3 = gcnew ClippingEdge(gcnew Vector(-X_ext, Y_ext, -Z_ext), gcnew Vector(-X_ext, Y_ext, Z_ext));
	cp3->worldFaceNormal = Vector::down;

	ClippingEdge^ cp4 = gcnew ClippingEdge(gcnew Vector(-X_ext, Y_ext, Z_ext), gcnew Vector(X_ext, Y_ext, Z_ext));
	cp4->worldFaceNormal = Vector::down;

	clipping->Add(cp1);
	clipping->Add(cp2);
	clipping->Add(cp3);
	clipping->Add(cp4);

	List<Vector^>^ points = gcnew List<Vector^>();
	float px = 0.3f;
	float pz = 0.4f;
	Vector^ p1 = gcnew Vector(0.f, Y_ext, 5.f);
	Vector^ p2 = gcnew Vector(0.f, Y_ext, -7.f);
	Vector^ p3 = gcnew Vector(-px, Y_ext, -pz);
	Vector^ p4 = gcnew Vector(-px, Y_ext, pz*1.2f);
	points->Add(p1);
	points->Add(p4);
	points->Add(p3);
	points->Add(p2);

	for (int i = 0; i < points->Count; i++)
	{
		int j = i + 1;
		if (j >= points->Count)
			j = 0;
		Draw::DebugLine(points[i], points[j], Colour::Yellow, Colour::Red);
		points[i]->DrawAsPointRaw(Colour::Yellow, 0.2f);
	}

	List<Vector^>^ output = gcnew List<Vector^>();
	cli::array<Vector^>^ clipPoints = points->ToArray();
	bool disable = false;
	Vector^ centre = gcnew Vector();
	for (int i = 0; i < clipping->Count; i++)
	{
		centre += clipping[i]->a;
	}
	centre /= (float)clipping->Count;

	int cpcount = 0;
	for each(ClippingEdge^ cp in clipping)
	{
		if (disable)
			break;
		cpcount++;

		float co = cp->GetEdgeNormal()->dot(cp->a - centre);
		if (co > 0.f)
		{
			Vector^ tmp = cp->a;
			cp->a = cp->b;
			cp->b = tmp;
		}

		cp->Draw("Cp" + cpcount, Colour::Yellow, Colour::Green);
		output->Clear();

		for (int i = 0; i < clipPoints->Length; i++)
		{
			if (clipPoints->Length == 0)
				break;
			int j = i + 1;
			if (j >= clipPoints->Length)
				j = 0;

			Vector^ p = clipPoints[i];
			Vector^ pn = clipPoints[j];

			if (Vector::InsidePlane3d(cp->GetEdgeNormal(), cp->a, p))
			{
				output->Add(p);
				if (!Vector::InsidePlane3d(cp->GetEdgeNormal(), cp->a, pn))
				{
					Vector^ newPoint = Vector::RayPlaneIntersection(p, pn, cp->a, cp->GetEdgeNormal());
					if (newPoint != nullptr)
					{
						output->Add(newPoint);
					}
				}
				else
				{
					output->Add(pn);
				}
			}
			else {
				if (Vector::InsidePlane3d(cp->GetEdgeNormal(), cp->a, pn))
				{
					Vector^ newPoint = Vector::RayPlaneIntersection(p, pn, cp->a, cp->GetEdgeNormal());
					if (newPoint != nullptr)
					{
						output->Add(newPoint);
					}
					output->Add(pn);
				}
			}
		}
		clipPoints = output->ToArray();
	}

	if (!disable)
	{
		for (int i = 0; i < clipPoints->Length; i++)
		{
			int j = i + 1;
			if (j >= clipPoints->Length)
				j = 0;
			Draw::DebugLine(clipPoints[i], clipPoints[j], Colour::Purple);
			clipPoints[i]->DrawAsPointRaw(Colour::New, 0.1f);
		}
	}
}

//http://allenchou.net/2014/01/game-physics-stability-warm-starting/
//need to implement feature caching to points are maintained between frames
void Antimatter::Games::Manifold::PreSolve()
{
	for (int i = 0; i < contacts->Count; i++)
	{
		contacts[i]->PreSolve();
	}
}

void Antimatter::Games::Manifold::Solve()
{
	
	for (int i = 0; i < contacts->Count; i++)
	{
		contacts[i]->SolveSequential();
	}
}

void Antimatter::Games::Manifold::Debug()
{
	if (contact == nullptr)
		return;
	debug = true;
	CreateOneShot(contact);
	debug = false;
	contacts->Clear();
}

bool Antimatter::Games::Manifold::IsBroadphaseOverlapping()
{
	return broadPhaseOverlap = this->AABBOverlap();
}

bool Antimatter::Games::Manifold::IsOutside(Vector^ a, Vector^ b, Vector ^ point)
{
	Vector^ tmp1 = b - a;
	Vector^ tmp2 = point - b;

	float x = tmp1->dot(tmp2);
	if (x < 0.f)
	{
		return false;
	}
	else if (x > 0.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

