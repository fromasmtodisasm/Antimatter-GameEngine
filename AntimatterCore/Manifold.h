#pragma once

#include "Contact.h"
#include "Mesh.h"
#include "PhysicsPair.h"
#include "MPR.h"

#define MANIFOLD_CACHE_SIZE 4
#define B3_LARGE_FLOAT 1e18f

//https://pastebin.com/dfE29kR5
//https://stackoverflow.com/questions/16389217/finding-the-intersection-of-two-3d-polygons

//clipping
//http://www.sunshine2k.de/coding/java/SutherlandHodgman/SutherlandHodgman.html

//guass maps
//http://www.gdcvault.com/play/1017646/Physics-for-Game-Programmers-The
//HAL documentation of gauss maps
//https://hal.archives-ouvertes.fr/hal-01157239/document

//convex convex collision detection issues thrwead
//https://pybullet.org/Bullet/phpBB3/viewtopic.php?p=&f=4&t=1918

//sources
//https://github.com/irlanrobson/bounce_lite/blob/master/Src/Collision/b3SAT.cpp
//https://github.com/JoshClose/gamecode4/blob/master/Source/GCC4/Physics/Physics.cpp

//forum help
//https://www.gamedev.net/forums/topic/510154-gjk-termination/
//https://www.gamedev.net/forums/topic/667499-3d-sat-problem/?page=2
//https://pybullet.org/Bullet/phpBB3/viewtopic.php?f=4&t=288&start=15
//https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=288

//pdfs
//https://pdfs.semanticscholar.org/476d/fce4be549655938c499663af246702cbc781.pdf
//http://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physics5collisionmanifolds/2017%20Tutorial%205%20-%20Collision%20Manifolds.pdf
//https://www.scss.tcd.ie/Michael.Manzke/CS7057/cs7057-1516-08-ContactModelling-mm.pdf

//http://www.sunshine2k.de/coding/java/SutherlandHodgman/SutherlandHodgman.html

namespace Antimatter {
	namespace Games {

		public ref class ClippingEdge
		{
		public:
			ClippingEdge()
			{
				int indexA=0;
				int indexB=0;
				a = gcnew Vector();
				b = gcnew Vector();
				faceNormal = gcnew Vector();
				worldFaceNormal = gcnew Vector();
			};

			int indexA;
			int indexB;
			Vector^ a;
			Vector^ b;
			Vector^ faceNormal;
			Vector^ worldFaceNormal;
			Mesh^ mesh;
			MPR^ mpr;
			
			ClippingEdge(Vector^ vec1,Vector^ vec2,int A, int B,Vector^ n)
			{
				indexA = A;
				indexB = B;
				faceNormal = n;
				a = vec1;
				b = vec2;
			}

			ClippingEdge(Vector^ vec1, Vector^ vec2)
			{
				a = vec1;
				b = vec2;
			}

			ClippingEdge(int A, int B, Vector^ n)
			{
				indexA = A;
				indexB = B;
				faceNormal = n;
			}

			bool isEdge(int A, int B)
			{
				if (indexA == A && indexB == B)
					return true;
				return false;
			}

			bool isReverseEdge(int A, int B)
			{
				if (indexA == B && indexB == A)
					return true;
				return false;
			}

			bool isReverseEdge(ClippingEdge^ edge)
			{
				if (indexA == edge->indexB && indexB == edge->indexA)
					return true;
				return false;
			}

			void Draw(Colour^ col)
			{
				Draw::DebugLine(a, b, Colour::Green, col);
				a->DrawAs("a", col);
			}

			void Draw(String^ text,Colour^ col,Colour^ col2)
			{
				Draw::DebugLine(a, b, col, col2);
				a->DrawAs(text, col);
			}

			void Draw(String^ text, Colour^ col, Colour^ col2,float size)
			{
				Draw::DebugLine(a, b, col, col2);
				a->DrawAs(text, col,size);
			}

			void Draw(Colour^ col,Colour^ col2)
			{
				Draw::DebugLine(a, b, col, col2);
			}
			void Draw(Colour^ col, Colour^ col2,Vector^ offset)
			{
				Draw::TextInWorld(indexA.ToString(), mesh*mesh->shape->points[indexA]+offset, col, 0.5f, 0.5f, true);
				Draw::TextInWorld(indexB.ToString(), mesh*mesh->shape->points[indexB]+offset, col2, 0.5f, 0.5f, true);
				Draw::DebugLine(mesh*mesh->shape->points[indexA], mesh*mesh->shape->points[indexB], col, col2);
			}

			static void DrawEdges(List<ClippingEdge^>^ edges,Colour^ col,Vector^ offset)
			{
				for (int i = 0; i < edges->Count; i++)
				{
					edges[i]->Draw(col, col,offset);
				}
			}

			static void DrawEdges(List<Face^>^ faces, Transform^ t,Colour^ col)
			{
				for each(Face^ f in faces)
				{
					f->draw(t, col);
				}
			}

			static void DrawEdges(List<Vertex^>^ edges, Colour^ col, Vector^ offset)
			{
				for (int i = 0; i < edges->Count; i++)
				{
					int j = i + 1;
					if (j >= edges->Count)
						j = 0;
					edges[i]->DrawTo((Vector^)edges[j],col, col);
				}
			}

			void DrawEdgeNormal(Colour^ col)
			{
				Vector^ v = GetEdgeNormal();
				a->DrawTo(a+v, col,col);
			}

			Vector^ GetEdgeNormal()
			{
				Vector^ edgevec = a - b;
				Vector^ edgeNorm = Vector::Cross(worldFaceNormal, edgevec);
				return edgeNorm;
			}
		};

		public ref class Manifold : PhysicsPair
		{
		private:
			void Construction()
			{
				contacts = gcnew List<Contact^>();
				mpr = gcnew MPR();
			};

		public:
			
			Manifold(Mesh ^ m1, Mesh ^ m2);
			List<Face^>^ GetSurface(Vector ^ direction, Mesh^ mesh);
			List<Vertex^>^ GetClippingPoints(List<Face^>^ faces,Mesh^ mesh);
			List<ClippingEdge^>^ GetClippingPlanes(List<Face^>^ faces,Mesh^ mesh);
			bool IsClockwise(List<Vertex^>^ polygon);
			List<Vertex^>^ GetIntersectionPolygon(List<Face^>^ clippedPolygon, Mesh^ inc_Mesh,List<Face^>^ referenceEdges, Mesh^ ref_Mesh);
			void CreateOneShot(Contact ^ c);
			void TestClipping();
			void PreSolve();
			void Solve();
			void Debug();
			bool IsBroadphaseOverlapping();
			bool inNarrowphase;
			bool IsOutside(Vector^ a, Vector^ b, Vector^ point);
			Mesh^ A;
			Mesh^ B;
			MPR^ mpr;
			Vector^ normal;
			List<Contact^>^ contacts;
			Contact^ deepContact;
			Contact^ contact;
			void Update(Contact^ c);
			bool resting;
			bool broadPhaseOverlap;
			bool debug;
			bool colliding;

			List<Vertex^>^ IterateEdgesClockwise(List<Vertex^>^ polygon)
			{
				if (IsClockwise(polygon))
				{
					//Already clockwise
					return polygon;
				}
				else
				{
					polygon->Reverse();
					return polygon;
				}
			}
		};
	}
}

