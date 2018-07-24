#pragma once
#include "Scene.h"
#include "Engine.h"

namespace Antimatter
{
	namespace Games
	{
		public ref class PhysicsScene : Scene
		{
		public:
			PhysicsScene() : Scene()
			{
				Load();
			};

			Engine^ game;
			Keyboard^ keyboard;

			PhysicsScene(Engine^ e) : Scene()
			{
				game = e;
				renderer = Render::current;
				Load();
			}

			void Load() override
			{
				Vector^ lightpos = gcnew Vector(0.f, 5.f, 10.f);
				keyboard = Keyboard::defaultKeyboard;
				//MousePositionEventHandler^ mpeh = gcnew MousePositionEventHandler(this, &PhysicsScene::MouseMove);
				//Engine::SetMouseMoveFunction(mpeh);
				//MouseEventHandler^ meh = gcnew MouseEventHandler(this, &PhysicsScene::OnMouse);
				//Engine::SetMouseFunction(meh);
				//keyboard = Keyboard::defaultKeyboard;
			}

			Vector^ startPos = gcnew Vector(0.5f, 10.0f, 0.0f);
			Mouse^ ms = Mouse::Default;
			int count = 0;
			bool disco = false;
			bool enableCollisionFeedback = false;
			bool autoStep = false;
			int stepInterval = 50;

			void UpdateScene() override
			{
				DrawThings();
			//	keyboard->keyDelay++;
	
				if (keyboard->IsPressed(KeyButton::B))
				{
					Physics::Break = !Physics::Break;
				}

				if (keyboard->IsPressed(KeyButton::D) && hoveredThing != nullptr)
				{
					if (dynamic_cast<Mesh^>(hoveredThing) != nullptr)
					{
						Mesh^ m = (Mesh^)hoveredThing;
						m->physics->debug = !m->physics->debug;
					}
				}

				if (keyboard->IsDown(KeyButton::O))
				{
					if (hoveredThing != nullptr)
						hoveredThing->SetColour(Colour::New);
				}

				DebugSelectedThing();
			}

			Vector^ dragoffset;
			Thing^ lastHoveredThing;
			Mesh^ selectedMesh;
			Mouse^ mouseState;
			Mesh^ cube;

			void OnMouse(Mouse^ m)
			{
				mouseState = m;
				//editorMouse->mouse->SetState(m);
			}

			void DrawScale()
			{
				Draw::TextInWorld("origin", Vector::Zero, Colour::White, 5, 5, true);
				Draw::TextInWorld(".", Vector::Zero, Colour::Red, 5, 5, true);
			}

			void DebugSelectedThing()
			{
				if (selectedMesh != nullptr)
				{
					if (selectedMesh->physics->debug == false)
					{

					}
					else
					{
						Draw::TextInWorld(selectedMesh->name, selectedMesh->Position, Colour::White, 1, 1, true);
					}
				}
				if (hoveredThing != nullptr)
				{
					//Draw::TextBillboardInWorld(hoveredThing->name, hoveredThing->Position, Colour::White, 1, 1);
					Draw::TransformBillboard(hoveredThing->name, hoveredThing, Vector::left, hoveredThing->colour, 1, 1);
					if (keyboard->IsPressed(KeyButton::D))
					{
						if (hoveredThing != nullptr)
						{
							if (dynamic_cast<Mesh^>(hoveredThing) != nullptr)
							{
								Mesh^ m = (Mesh^)hoveredThing;
								m->physics->debug = !m->physics->debug;
								if (m->physics->debug)
									selectedMesh = m;
								else
									selectedMesh = nullptr;
								return;
							}
						}
					}
				}
			}

			Thing^ hoveredThing;
			Thing^ grabbedThing;

			void MouseMove(double x, double y)
			{
				/*
				GetHoveredThing(x, y);
				editorMouse->mouse->x = x;
				editorMouse->mouse->y = y;
				editorMouse->mouse->clientX = MouseState::mouseState->clientX;
				editorMouse->mouse->clientY = MouseState::mouseState->clientY;
				*/
			}

			void GetHoveredThing(double x, double y)
			{
				hoveredThing = SelectThing(x, y, false);
				if (hoveredThing != nullptr)
				{
					lastHoveredThing = hoveredThing;
					return;
				}
				else
				{
					if (lastHoveredThing != nullptr)
					{
						//  lastHoveredThing.colour = lastHoveredThing.defaultColor;
						lastHoveredThing = nullptr;
					}
				}
			}
		};
	}
}


