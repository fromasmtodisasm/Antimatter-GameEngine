#include "PhysicsScene2D.h"

using namespace Antimatter::Games;

//http://in2gpu.com/2014/05/12/gjk-algorithm-collision-detection-2d-in-c/
//https://hamaluik.com/posts/building-a-collision-engine-part-1-2d-gjk-collision-detection/

//calculate simplex winding and penetration depth
//https://hamaluik.com/posts/building-a-collision-engine-part-2-2d-penetration-vectors/


//gjk 1d visualization
//https://github.com/kroitor/gjk.c

//gjk 3d
//http://in2gpu.com/2014/05/18/gjk-algorithm-3d/
//epa 2d
//http://kumarcode.com/2D-GJK-and-EPA/


PhysicsScene2D::PhysicsScene2D() : PhysicsScene()
{

}

void PhysicsScene2D::Load()
{
	PhysicsScene::Load();
	box1 = gcnew Collider2d("box", Shape2D::Box);
	box2 = gcnew Collider2d("triangle", Shape2D::Triangle);
	ground = gcnew Collider2d("box", Shape2D::Box);
	ground->transform->position = gcnew Vector(0.f, -10.f, 0.f);
	ground->transform->scale *= gcnew Vector(10, 0.5f);
	camera->eye->Position += gcnew Vector(0.f, 0.f, -20.f);
	box1->transform->position = gcnew Vector(0.8f, -2.f, 0.f);
	box2->transform->position = gcnew Vector(0.f, 0.5f, 0.f);
	box2->transform->rotation = 45.f * math_to_radians;

	physics1 = gcnew Physics2d(box1);
	physics2 = gcnew Physics2d(box2);
	gPhysics = gcnew Physics2d(ground);

	gPhysics->SetMassProperties(0.f);
	gPhysics->gravity = Vector::Zero;
	physics2->gravity *= 0.08f;
	physics1->SetMassProperties(50.f);
	physics2->SetMassProperties(10.f);
	physics2->restitution = .5f;
	physics1->restitution = .0f;

	solver = gcnew PhysicsSolver2d();
	solver->bodies->Add(gPhysics);
	solver->bodies->Add(physics1);
	solver->bodies->Add(physics2);
	
	box1->colour = Colour::Orange;
	box2->colour = Colour::Yellow;
	ground->colour = Colour::Vermillion;

	//call update to set world points before rendering
	ground->Update();
	box1->Update();
	box2->Update();
}

void PhysicsScene2D::Update()
{
	PhysicsScene::UpdateScene();

	if (keyboard->IsDown(KeyButton::B))
	{
		solver->step = true;
	}

	if (keyboard->IsDown(KeyButton::E))
	{
		solver->enableBreaking = !solver->enableBreaking;
	}

	if(solver->enableBreaking)
		Draw::Text(-1, 0.5f, "Breaking enabled: true");

	if (cube != nullptr)
	{
		cube->Position = physics1->position;
		cube->EulerAngles = gcnew Vector(0.f, 0.f, physics1->orientation*math_to_degrees);
	}

	if (keyboard->IsPressed(KeyButton::P))
	{
		solver->pause = !solver->pause;
	}

	Vector::Zero->DrawAs("Origin", Colour::White);
	//Vector::left->DrawAs("Left", Colour::White);
	//(Vector::Zero - gcnew Vector(0.f,5.f,0.f))->DrawAs("Test Text", Colour::Red);
	//gcnew WorldText("Test Text", Colour::Red, Vector::Zero - gcnew Vector(0.f,5.f,0.f), 1.f, 1.f, true);
	box1->Draw();
	box2->Draw();
	ground->Draw();

	if (!solver->pause || solver->step)
	{
		solver->Step();
		solver->step = false;
	}

	for each(Manifold2d^ m in solver->manifolds)
	{
		if (m->impact)
		{
		}
	}

	count++;
	if (count > 45)
	{
		physics2->torque = 5.f;
		count = 0;
	}

	if (solver->manifolds->Count == 1)
	{
		if(solver->enableBreaking)
			solver->pause = true;
	}

	ground->Update();
	box1->Update();
	box2->Update();

	Graphic::DrawBatches();
}

void PhysicsScene2D::DrawAxis2D(Vector^ pos)
{
	Draw::DebugVector(pos, Vector::front, Colour::Blue);
	//Draw::TextBillboardInWorld("0", Vector::Zero, Colour::White, 1.f);
	Draw::DebugVector(pos, Vector::up, Colour::Green);
	//Draw::TextBillboardInWorld(Vector::up->GetString(), Vector::up, Colour::White, 1.f);
	Draw::DebugVector(pos, Vector::right, Colour::Purple);
	//Draw::TextBillboardInWorld(Vector::right->GetString(), Vector::right, Colour::White, 1.f);
}