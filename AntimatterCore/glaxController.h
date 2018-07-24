#pragma once

#include "GlaxionHeader.h"
#include "SystemHeader.h"
#include "Vector.h"

namespace Antimatter{
	namespace Games{

	namespace Inputs
	{
		public ref class Controller
		{
		public:

			String^ Name;
			float frame;
			float Frame;
			float LeftStickX;
			float LeftStickY;
			float RightStickX;
			float RightStickY;
			float Triggers;
			bool A;
			bool B;
			bool X;
			bool Y;
			bool RB;
			bool LB;
			bool START;
			bool BACK;
			bool Up;
			bool Down;
			bool Left;
			bool Right;
			bool R3;
			bool L3;
			int PlayerNumber;
			Vector^ LeftStick;
			Vector^ RightStick;
			bool IsConnected;
			bool WasConnected;

			Controller()
			{

			}

			Controller(int player)
			{
				if (player < 1)
					player = 1;
				else if (player > 4)
					player = 4;
				PlayerNumber = player;
			}

			bool Update()
			{
				IsConnected = glfwJoystickPresent(PlayerNumber - 1);
				if (WasConnected && !IsConnected)
				{
					MessageBox::Show(String::Concat("Controller ", PlayerNumber, " Diconnected"));
					WasConnected = false;
				}

				if (IsConnected)
				{
					if (!WasConnected)
						WasConnected = true;
					int axiscount;
					const float * axis = glfwGetJoystickAxes(PlayerNumber - 1, &axiscount);
					int buttoncount;
					const unsigned char* buttons = glfwGetJoystickButtons(PlayerNumber - 1, &buttoncount);
					LeftStickX = axis[0];
					LeftStickY = axis[1];
					RightStickX = axis[4];
					RightStickY = axis[3];
					A = buttons[0];
					B = buttons[1];
					X = buttons[2];
					Y = buttons[3];
					RB = buttons[5];
					LB = buttons[4];
					START = buttons[7];
					BACK = buttons[6];
					L3 = buttons[8];
					R3 = buttons[9];
					Up = buttons[10];
					Down = buttons[12];
					Left = buttons[13];
					Right = buttons[11];

					Triggers = axis[2];
					return true;
				}
				else{
					return false;
				}
			}

			void DebugController()
			{
				if (Update())
				{
					cout << "Left Stick X: " << LeftStickX << "\n";
					cout << "Left Stick Y: " << LeftStickY << "\n";
					cout << "Right Stick X: " << RightStickX << "\n";
					cout << "Right Stick Y: " << RightStickY << "\n";
					cout << "Triggers: " << Triggers << "\n";
					cout << "A: " << A << "\n";
					cout << "B " << B << "\n";
					cout << "X: " << X << "\n";
					cout << "Y: " << Y << "\n";
					cout << "LB: " << LB << "\n";
					cout << "RB: " << RB << "\n";
					cout << "START: " << START << "\n";
					cout << "BACK: " << BACK << "\n";
					cout << "L3: " << L3 << "\n";
					cout << "R3: " << R3 << "\n";
					cout << "Up: " << Up << "\n";
					cout << "Down: " << Down << "\n";
					cout << "Left: " << Left << "\n";
					cout << "Right: " << Right << "\n";
				}
				else{
					cout << "Controller not connected\n";
				}
			}

			String^ GetControllerName()
			{
				const char* name = glfwGetJoystickName(PlayerNumber);
				return gcnew String(name);
				//TODO
				//Convert to Managed string;
			}
		};
	}
	}
}