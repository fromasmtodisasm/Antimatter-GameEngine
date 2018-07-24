#pragma once
#include "PhysicsScene.h"
#include "PhysicsSolver3D.h"
#include "MPR.h"
#include "GJK3d.h"
#include "GJK2.h"

namespace Antimatter {
	namespace Games {

		public ref class PhysicsScene3D : PhysicsScene
		{
		public:

			Mesh^ cube;
			Mesh^ ground;
			Mesh^ thingA = cube;
			Mesh^ thingB = block;
			Vector^ stickyPoint;
			PhysicsSolver3D^ solver3d = gcnew PhysicsSolver3D();

			PhysicsScene3D() : PhysicsScene()
			{
				//Scene::MakeCurrent(this);
				block = AddMesh("Assets\\cube.x");
				block->Scale *= 2.f;
				block->Position += gcnew Vector(0.f, 0.f, 5.f);

				camera->eye->Position = gcnew Vector(0, 0, -15.f);

				cube = AddMesh("Assets\\cube.x");

				ground = AddMesh("Assets\\cube.x");
				ground->Scale *= 5.f;
				ground->Scale += gcnew Vector(100.f, 0, 100.f);
				ground->Position = gcnew Vector(0.f, -10.f, 0.f);

				cube->Position = gcnew Vector(2.f, 5.5f, 5.f);
				cube->EulerAngles = gcnew Vector(0.f, 0.f, 45.f);

				solver3d->AddThing(cube);
				solver3d->AddThing(block);
				solver3d->AddThing(ground);
				
				cube->physics->SetMassProperties(10.f);
				block->EulerAngles = gcnew Vector(0.f, 0.f, 25.f);
				block->physics->SetMassProperties(20.f);
				stickyPoint = gcnew Vector(1.f,0.f, 0.f);
				ground->name = "Ground";
				ground->physics->gravity = Vector::Zero;
				ground->physics->SetMassProperties(1000000000.f);
				ground->physics->SetDamping(1.f);
				//cube->physics->debug = true;
				block->name = "Block";
				//thingA = cube;
				//thingB = ground;
				ground->colour = Colour::White;
				//Load();

				
			//	ground->physics->frozen = true; //hack to prevent ground exploding
			}

			int timer = 0;
			bool enableBreaking = false;
			bool drawGJK = false;
			float g = 0.f;
			Vector^ mouseInWorld;
			
			void SpawnCube()
			{
				Mesh^ m = AddMesh("Assets\\cube.x");
				if (mouseInWorld == nullptr)
					m->Position = gcnew Vector(0.f, 5.f, 0.f);
				else
					m->Position = mouseInWorld;
				m->Scale *= gcnew Vector(2.f, 1.f, 1.f);
				m->EulerAngles = gcnew Vector(45.f, 0.f, 45.f);
				m->name = "cuboid";
				solver3d->AddThing(m);
				m->physics->SetMassProperties(10.f);
				gameObjects->Add(m);
			}

			bool switchdebugthing;
			void SelectThingForDebugging()
			{
				if (keyboard->IsPressed(KeyButton::D))
				{
					Mesh^ m = SelectThing((float)editorMouse->mouse->x, (float)editorMouse->mouse->y, false);
					if (m != nullptr)
					{
						m->physics->debug = !m->physics->debug;
					}

					if (thingA == m)
					{
						thingA = nullptr;
						return;
					}
					if (thingB == m)
					{
						thingB = nullptr;
						return;
					}

					if (thingA == nullptr)
					{
						thingA = m;
						return;
					}
					if (thingB == nullptr)
					{
						thingB = m;
						return;
					}
					if (m != thingA && !switchdebugthing)
					{
						thingA = m;
						switchdebugthing = true;
						return;
					}
					if (m != thingB && switchdebugthing)
					{
						thingB = m;
						switchdebugthing = false;
					}
				}
			}

			void Update() override
			{

				solver3d->Step();

				/*
				Manifold^ findManifold = solver3d->FindManifold(thingA, thingB);
				if (findManifold != nullptr)
				{
					Mesh^ a = findManifold->A;
					Mesh^ b = findManifold->B;
					Vector^ v = (b->Position - a->Position)*0.5f;
					v->DrawAs(findManifold->GetString(),(b->colour - a->colour)*0.5f);
					solver3d->DebugPair(a, b);
				}
				*/
			//	Vector^ v = mouse->GetClientPostion();
				//Draw::Text(0.f, 0.f, "X: " + v->x + " Y: " + v->y);
				Vector^ v = gcnew Vector((float)editorMouse->mouse->x, (float)editorMouse->mouse->y, 0.f,1.f);
				
				Vector^ r = Render::Get3DPoint(camera, v->x, v->y, .99993f);
				
				mouseInWorld = (camera->eye->Position - r);
				cube->colour = Colour::Blue;
				block->colour = Colour::Red;
				static bool DrawHull = false;
				static bool DrawSimplex = false;

				if(thingA && thingB)
					solver3d->DebugPair(thingA, thingB);
				
				SelectThingForDebugging();
				/*
				if (keyboard->IsDown(KeyButton::W))
				{
					Thing^ t = SelectThing((float)editorMouse->mouse->x, (float)editorMouse->mouse->y, false);
					if (t == thingB)
					{
						thingB = nullptr;
					}else if(t != nullptr && thingB != thingA)
						thingB->physics->debug = !thingB->physics->debug;
				}
				*/

				if(keyboard->IsPressed(KeyButton::O))
					Contact::ShowFriction = !Contact::ShowFriction;

				if(keyboard->IsPressed(KeyButton::H))
					DrawHull = !DrawHull;

				if (keyboard->IsPressed(KeyButton::J))
					DrawSimplex = !DrawSimplex;

				if (keyboard->IsDown(KeyButton::E))
					enableBreaking = !enableBreaking;

				if (keyboard->IsDown(KeyButton::S))
					solver3d->step = !solver3d->step;

				if (keyboard->IsDown(KeyButton::G))
					drawGJK = !drawGJK;

				if (keyboard->IsPressed(KeyButton::P))
					Physics::Break = !Physics::Break;

				if (keyboard->IsPressed(KeyButton::C))
				{
					SpawnCube();
				}

				if (keyboard->IsDown(KeyButton::N))
					SpawnCube();

				if (enableBreaking)
				{
					Draw::Text(-1.f, 0.5f, "Enable Breaking");
				}
				
				if (DrawSimplex)
				{
					GJK3D^ gjk3d = gcnew GJK3D();
					gjk3d->Test(thingA, thingB);
					gjk3d->DebugSimplex();
				}

				if (DrawHull && thingA != nullptr && thingB != nullptr)
				{
					GJKDistance^ distance = gcnew GJKDistance();
					distance->A = thingA;
					distance->B = thingB;
					distance->DrawHull(Colour::Blue, 0.01f);
				}
				
				DrawThings();
				Graphic::DrawBatches();
			}
		};
	}
}

