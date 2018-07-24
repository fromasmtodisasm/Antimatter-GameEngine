#pragma once
#include "Keyboard.h"
#include "Draw.h"

namespace Antimatter {
	namespace Games {

		public ref class SubScene
		{
		public:
			SubScene()
			{

			}

			bool pause;
			bool step;
			bool enableBreaking;
			Keyboard^ keyboard = Keyboard::defaultKeyboard;

			virtual void Load()
			{

			}

			void BreakableUpdate()
			{
				if (keyboard->IsDown(KeyButton::B))
				{
					step = true;
				}

				if (keyboard->IsDown(KeyButton::E))
				{
					enableBreaking = !enableBreaking;
				}

				if (enableBreaking)
					Draw::Text(-1, 0.5f, "Breaking enabled: true");

				if (keyboard->IsPressed(KeyButton::P))
				{
					pause = !pause;
				}

				if (!pause || step)
				{
					Update();
					step = false;
				}
			}

			virtual void Update()
			{

			}

			void ForceRun()
			{
				pause = false;
				step = false;
			}
		};
	}
}



