#pragma once
#include "PhysicsScene.h"
#include "SubScene.h"
#include "PhysicsSolver2d.h"
#include "Sound.h"

//https://hamaluik.com/posts/building-a-collision-engine-part-2-2d-penetration-vectors/


namespace Antimatter {
	namespace Games {
		
		public ref class PhysicsScene2D : PhysicsScene
		{
		public:

			PhysicsScene2D();
			virtual void Load() override;
			virtual void Update() override;
			void DrawAxis2D(Vector ^ pos);
			List<Vertex^>^ simplices = gcnew List<Vertex^>();
			Vector^ pos1;
			Vector^ pos2;
			bool step = false;
			bool breakMode;
			bool enableBreakMode;
			bool isCollision;
			int count;
			Sound^ s = gcnew Sound("Assets\\The Alien part 1.mp3");
			GJK2D^ subscene;
			Collider2d^ box1;
			Collider2d^ box2;
			Collider2d^ ground;
			Physics2d^ physics1;
			Physics2d^ physics2;
			Physics2d^ gPhysics;
			PhysicsSolver2d^ solver;
			Mesh^ cube2;

			void MoveVertex2d(Vector^% v,float speed)
			{
				if (keyboard->IsDown(KeyButton::Right))
					v->x -= speed;
				if (keyboard->IsDown(KeyButton::Left))
					v->x += speed;

				if (keyboard->IsDown(KeyButton::Up))
					v->y += speed;
				if (keyboard->IsDown(KeyButton::Down))
					v->y -= speed;
			}
		};
	}
}

