#pragma once
#include "Engine.h"
#include "PhysicsScene2D.h"
#include "Sound.h"
#include "MouseShooter.h"
#include "PhysicsScene3D.h"

namespace Antimatter
{
	namespace Games
	{
		public ref class TestGame : Engine
		{
		public:
			TestGame() : Engine()
			{

			}

			PhysicsScene2D^ scene2D;
			PhysicsScene3D^ scene3D;
			MouseShooter^ mouseShooterScene;
			
			int sceneIndex;
			int delay = 0;

			void Load() override
			{
				mouseShooterScene = gcnew MouseShooter();
				scene2D = gcnew PhysicsScene2D();
				scene3D = gcnew PhysicsScene3D();

				scenes->Add(scene3D);
				scenes->Add(mouseShooterScene);
				scenes->Add(scene2D);
				
				sceneIndex = 0;
			}

			void Update(float deltaTime) override
			{
				keyboard->keyDelay++;
				scenes[sceneIndex]->Update();
				
				if (keyboard->IsPressed(KeyButton::Q))
					IncrementScene();

				float frameLine = 0.9f;

				Colour^ col = Colour::Red;
				if (FPS >= 60.0f)
					col = Colour::LightBlue;
				else if (FPS < 60.0f && FPS > 30.0f)
					col = Colour::Green;
				else if (FPS < 30.0f)
					col = Colour::Vermillion;

				Draw::Text(-1.f, frameLine, col, 1.f, 1.f, FPS.ToString());
			}

			void IncrementScene()
			{
				sceneIndex++;
				if (sceneIndex > scenes->Count-1)
				{
					sceneIndex = 0;
				}
			}
		};
	}
}