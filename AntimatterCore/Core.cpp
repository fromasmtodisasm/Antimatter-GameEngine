#include "Core.h"
#include "glaxCore.h"

namespace Antimatter
{
	namespace Games
	{
		void Core::MouseButton_CallBack(int button, int action, int mods)
		{
			CoreEvents->OnMouseClickEvent(button, action, mods);
			//MouseState::GlobalState->button = (MouseButton)button;
			//MouseState::GlobalState->action = (MouseAction)action;
			//CoreEvents->OnMouseEvent(MouseState::GlobalState);
		}

		void Core::MousePosition_CallBack(double x, double y)
		{
			//MouseState::GlobalState->x = x;
			//MouseState::GlobalState->y = y;
			CoreEvents->OnMoveEvent(x, y);
		}

		void Core::MouseScroll_CallBack(int deltax, int deltay)
		{
			//MouseState::GlobalState->scroll = deltay;
			CoreEvents->OnScrollEvent(deltax, deltay);
		}

		glm::vec2 Core::GetNDC2D(float x, float y)
		{
			float normalizedX = -1.0f + 2.0f * x / (float)glx::Core::windowWidth;
			float normalizedY = 1.0f - 2.0f * y / (float)glx::Core::windowHeight;
			return glm::vec2(normalizedX, normalizedY);
		}
	}
}
