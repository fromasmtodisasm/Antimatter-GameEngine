#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"

namespace Antimatter {
	namespace Games
	{
		public enum class KeyButton :char {
			Null, Q, W, E, R, T, Y, U, I, O, P, A, S, D, F, G, H, J,
			K, L, Z, X, C, V, B, N, M, Enter, Up, Down, Left, Right, n0, n1, n2, n3, n4, n5, n6, n7,
			n8, n9, Tab, Space, Backspace, ShiftLeft, ShiftRight, Ctrl, Alt, Plus, Minus
		};

		public enum class MouseButton : char { Left, Right, Middle, Back, Front, SixthButton,None };

		public enum class MouseAction : char { Up, Down, Pressed, Released };

		public enum class ButtonAction : char { Up, Down, Pressed, Released };

		ref class Camera;
		public ref class Input
		{
		public:

			Input()
			{

			}

		};
	}
}
