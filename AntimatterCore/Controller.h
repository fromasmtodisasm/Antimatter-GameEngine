#pragma once

#include "GlaxionHeader.h"
#include "SystemHeader.h"
#include "Vector.h"

namespace Antimatter
{
	namespace Games
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
				
				if (WasConnected && !glfwJoystickPresent(PlayerNumber - 1))
				{
					IsConnected = false;
					tool::debug(String::Concat("Controller ", PlayerNumber, " Diconnected"));
					return false;
				}
				
				int axiscount;
				const float * axis = glfwGetJoystickAxes(PlayerNumber - 1, &axiscount);
				int buttoncount;
				const unsigned char* buttons = glfwGetJoystickButtons(PlayerNumber - 1, &buttoncount);
				LeftStickX = axis[0];
				LeftStickY = axis[1];
				RightStickX = axis[4];
				RightStickY = axis[3];
				A = (bool)buttons[0];
				B = (bool)buttons[1];
				X = (bool)buttons[2];
				Y = (bool)buttons[3];
				RB = (bool)buttons[5];
				LB = (bool)buttons[4];
				START = (bool)buttons[7];
				BACK = (bool)buttons[6];
				L3 = (bool)buttons[8];
				R3 = (bool)buttons[9];
				Up = (bool)buttons[10];
				Down = (bool)buttons[12];
				Left = (bool)buttons[13];
				Right = (bool)buttons[11];

				Triggers = axis[2];
				return IsConnected = true;;
			}

			void DebugController()
			{
				if (Update())
				{
					cout << "Left Stick X: " << LeftStickX << endl;
					cout << "Left Stick Y: " << LeftStickY << endl;
					cout << "Right Stick X: " << RightStickX << endl;
					cout << "Right Stick Y: " << RightStickY << endl;
					cout << "Triggers: " << Triggers << endl;
					cout << "A: " << A << endl;
					cout << "B " << B << endl;
					cout << "X: " << X << endl;
					cout << "Y: " << Y << endl;
					cout << "LB: " << LB << endl;
					cout << "RB: " << RB << endl;
					cout << "START: " << START << endl;
					cout << "BACK: " << BACK << endl;
					cout << "L3: " << L3 << endl;
					cout << "R3: " << R3 << endl;
					cout << "Up: " << Up << endl;
					cout << "Down: " << Down << endl;
					cout << "Left: " << Left << endl;
					cout << "Right: " << Right << endl;
				}
				else{
					cout << "Controller not connected"<<endl;
				}
			}

			String^ GetControllerName()
			{
				const char* name = glfwGetJoystickName(PlayerNumber);
				return gcnew String(name);
			}
		};
	}
}

