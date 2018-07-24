#include "Mouse.h"

namespace Antimatter {
	namespace Games {

		bool Mouse::IsDown(MouseButton mb)
		{
			if (action == MouseAction::Down && button == mb)
			{
				return true;
			}
			else
				return false;
		}

		bool Mouse::IsReleased(MouseButton mb)
		{
			if (button != mb)
				return false;

			if (actionCode == 1)
			{
				isDown = true;
				return false;
			}

			if(isDown && actionCode == 0)
			{
				isDown = false;
				return true;
			}
		}

		bool Mouse::IsPressed(MouseButton mb)
		{
			if (button != mb)
				return false;

			if (actionCode == 0)
			{
				isUp = true;
				return false;
			}

			if (isUp && actionCode == 1)
			{
				isUp = false;
				return true;
			}
		}

		bool Mouse::IsUp(MouseButton mb)
		{
			if (action == MouseAction::Up && button == mb)
			{
				return true;
			}
			else
				return false;
		}
	}
}