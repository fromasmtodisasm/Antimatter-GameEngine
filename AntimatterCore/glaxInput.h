#pragma once

#include "SystemHeader.h"
#include "glaxVector.h"
#include "glaxCamera.h"

namespace Glaxion
{
	namespace Native
	{
		enum KeyboardButton :char { Null, Q, W, E, R, T, Y, U, I, O, P, A, S, D, F, G, H, J, K, L, Z, X, C, V, B, N, M, Enter, Up, Down, Left, Right, n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, Tab, Space, Backspace, ShiftLeft, ShiftRight, Ctrl, Alt };

		enum MouseButton : char{ Left, Right, Middle, Back, Front, SixthButton };

		enum ButtonAction : char{ Up, Down, Pressed, Released };

		class Input
		{
		public:
			static float Pan(float position, float lastposition, float rate, float threshold, float flip)
			{
				float pan = 0;

				if (position > lastposition + threshold)
					pan = (rate + position - lastposition) * flip;
				if (position < lastposition - threshold)
					pan = ((rate + lastposition - position)* flip) * -1;
				return pan;
			}
		};



		class Controller
		{
		public:

			const char* Name;
			float frame;
			float Frame;
			float Xleftstick;
			float Yleftstick;
			float Xrightstick;
			float Yrightstick;
			float Triggers;

			Vector LeftStick;
			Vector RightStick;

			Controller()
			{

			}

			void ReadAxisInput(int);

			static void GetControllerInput()
			{
				int count;
				const float * values;
				values = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

				/*
				for (int i; i < count; i++)
				{
				//cerr << "-> "<<values[i]<<"\n";
				}
				*/

				//cerr<<glfwGetJoystickName(GLFW_JOYSTICK_1);
			}

			static const char* GetControllerName()
			{
				return glfwGetJoystickName(GLFW_JOYSTICK_1);
			}
		};

		class Mouse
		{
		public:
			int mouseButton;
			int buttonAction;
			double x;
			double y;
			double deltaX;
			double deltaY;
			MouseButton Button;
			ButtonAction  Action;
			bool ButtonDown;
			bool Toggle;
			Vector LastPosition;
			Vector Position;

			Mouse()
			{
				x = 0;
				y = 0;
				LastPosition = Vector();
			};

			void CameraControl(Camera& cam)
			{

				if (Action == ButtonAction::Down)
				{
					//Console.WriteLine(Mouse.buttonAction + Mouse.Action.ToString());
					float x = 0.1f;
					float y = 0.1f;
					float thresh = 2.5f;

					Vector vec;

					if (Button == MouseButton::Left)
					{

						vec.x += Input::Pan(Position.x, LastPosition.x, x, thresh, 0.001f);
						vec.z += Input::Pan(Position.y, LastPosition.y, x, thresh, 0.001f);
						vec.y = 0;
						cam.target.position += (cam.target.getRightDirection * vec.x) + (cam.target.getForwardDirection * vec.z);
					}

					else if (Button == MouseButton::Middle)
					{
						float rotx = 0.03f;
						Vector rot = Vector();
						rot.y -= Input::Pan(Position.x, LastPosition.x, rotx, thresh + 50, 0.0001f);
						rot.x += Input::Pan(Position.y, LastPosition.y, rotx, thresh + 100, 0.0001f);
						cam.target.orientation += rot;
					}

					else if (Button == MouseButton::Right)
					{
						vec.x += Input::Pan(Position.x, LastPosition.x, x, thresh, 0.001f);
						
						//TODO does the height variable need to be on the camera?
						cam.height += Input::Pan(Position.y, LastPosition.y, x, thresh, -0.001f);
						cam.target.position += (cam.target.getRightDirection * vec.x) + (cam.target.getForwardDirection * vec.z);;
					}

					cam.target.position = Vector(cam.target.position.x, cam.height, cam.target.position.z);
				}
			}

			void ButtonClick(int button, int action, int mods)
			{
				mouseButton = button;
				buttonAction = action;

				if (button == 0)
					Button = MouseButton::Left;
				else if (button == 1)
					Button = MouseButton::Right;
				else if (button == 2)
					Button = MouseButton::Middle;
				else if (button == 3)
					Button = MouseButton::Back;
				else if (button == 4)
					Button = MouseButton::Front;

				switch (action)
				{
				case 0:
					if (ButtonDown)
						ButtonDown = false;
					Action = ButtonAction::Up;
					break;
				case 1:
					ButtonDown = true;
					Action = ButtonAction::Down;
					break;
				default:
					break;
				}

				if (action == 1)
					LastPosition = Position;

				cout << action;

				cerr<<"-> MouseButton: "<< Button<<"." <<Action;
			}

			void GetCursorPosition(double x, double y)
			{
				this->x = x;
				this->y = y;
			}

			void GetMouseWheel(double deltax, double deltay)
			{

				this->deltaX = deltax;
				this->deltaY = deltay;
			}

			void DebugButtons()
			{
				cerr<<Button<<" : " << Action;
			}
		};

		class Keyboard
		{
		public:

			KeyboardButton Key;
			ButtonAction Action;
			int delay;
			bool KeyDown;
			bool KeyUp;

			Keyboard()
			{

			}

			void ReadKeyboard(int key, int scancode, int action, int mods)
			{

				//cerr << "Key: " << key << "\n";
				switch (key)
				{
				case 81:
					Key = KeyboardButton::Q;
					break;
				case 87:
					Key = KeyboardButton::W;
					break;
				case 69:
					Key = KeyboardButton::E;
					break;
				case 82:
					Key = KeyboardButton::R;
					break;
				case 84:
					Key = KeyboardButton::T;
					break;
				case 89:
					Key = KeyboardButton::Y;
					break;
				case 85:
					Key = KeyboardButton::U;
					break;
				case 73:
					Key = KeyboardButton::I;
					break;
				case 79:
					Key = KeyboardButton::O;
					break;
				case 80:
					Key = KeyboardButton::P;
					break;
				case 65:
					Key = KeyboardButton::A;
					break;
				case 83:
					Key = KeyboardButton::S;
					break;
				case 68:
					Key = KeyboardButton::D;
					break;
				case 70:
					Key = KeyboardButton::F;
					break;
				case 71:
					Key = KeyboardButton::G;
					break;
				case 72:
					Key = KeyboardButton::H;
					break;
				case 74:
					Key = KeyboardButton::J;
					break;
				case 75:
					Key = KeyboardButton::K;
					break;
				case 76:
					Key = KeyboardButton::L;
					break;
				case 90:
					Key = KeyboardButton::Z;
					break;
				case 88:
					Key = KeyboardButton::X;
					break;
				case 67:
					Key = KeyboardButton::C;
					break;
				case 86:
					Key = KeyboardButton::V;
					break;
				case 66:
					Key = KeyboardButton::B;
					break;
				case 78:
					Key = KeyboardButton::N;
					break;
				case 77:
					Key = KeyboardButton::M;
					break;
				case 257:
					Key = KeyboardButton::Enter;
					break;
				case 265:
					Key = KeyboardButton::Up;
					break;
				case 264:
					Key = KeyboardButton::Down;
					break;
				case 263:
					Key = KeyboardButton::Left;
					break;
				case 262:
					Key = KeyboardButton::Right;
					break;
				case 48:
					Key = KeyboardButton::n0;
					break;
				case 49:
					Key = KeyboardButton::n1;
					break;
				case 50:
					Key = KeyboardButton::n2;
					break;
				case 51:
					Key = KeyboardButton::n3;
					break;
				case 52:
					Key = KeyboardButton::n4;
					break;
				case 53:
					Key = KeyboardButton::n5;
					break;
				case 54:
					Key = KeyboardButton::n6;
					break;
				case 55:
					Key = KeyboardButton::n7;
					break;
				case 56:
					Key = KeyboardButton::n8;
					break;
				case 57:
					Key = KeyboardButton::n9;
					break;
				case 258:
					Key = KeyboardButton::Tab;
					break;
				default:
					break;
				}

				switch (action)
				{
				case 0:	//button up
					if (KeyDown)
					{
						Action = ButtonAction::Released;
						KeyDown = false;
					}
					else
					{
						Action = ButtonAction::Up;
					}
					break;
				case 1:
					Action = ButtonAction::Pressed;
					KeyDown = true;
					break;
				case 2:
					Action = ButtonAction::Down;
					break;
				case 3:
					cerr << "\n-> Mouse Input class -> Forth mouse button state\n";
					break;
				default:
					break;
				}

				/*
				if (action == 0)
				{
				//Action = ButtonAction::Tapped;
				//Console::WriteLine("ACTION!");
				/*
				KeyDown = false;
				KeyUp = true;
				delay = 0;
				}else
				if (action == 1)
				{

				KeyDown = true;
				KeyUp = false;

				}

				//KeyUp = !KeyDown;

				if (KeyDown)
				{
				delay++;
				if (delay < 2)
				{

				Action = ButtonAction::Clicked;
				}
				else{

				Action = ButtonAction::Pressed;
				}
				}
				*/
				cerr<<Action<<".";
				cerr<<Key<<".";
				cerr<<"num: "<< key;
				//Console::Write(Key.ToString()+".");
				//if (Key != Input::KeyboardButton::Null)

				//	cerr<<"-> Keyboard key: " << Key.ToString;
			}
		};
	}
}