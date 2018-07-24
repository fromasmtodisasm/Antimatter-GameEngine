#pragma once

#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Camera.h"
#include "Input.h"
#include "Core.h"

#define KEY_DELAY = 10

namespace Antimatter {
	namespace Games {
		
		public ref struct Keys
		{
		public:

			Keys() {};

			Keys(int code)
			{
				keyCode = code;
			}

			Keys(const Keys% v)
			{
				keyCode = v.keyCode;
				keyActionCode = v.keyActionCode;
				Pressed = v.Pressed;
				Down = v.Down;
				Up = v.Up;
				Released = v.Released;
				keyDelay = v.keyDelay;
			}

			void Update(int action,int key)
			{
				keyActionCode = action;
				if (keyActionCode == 1)
				{
					Up = false;
					Pressed = true;
					Down = true;
					wasdown = true;
					return;
				}
				else if (keyActionCode == 2)
				{
					Pressed = false;
					Down = true;
					wasdown = true;
					return;
				}else
				if (keyActionCode == 0)
				{
					Up = true;
					if (Released)
					{
						
						if (keyDelay > 25)
						{
							Released = false;
							wasdown = false;
							keyDelay = 0;
							return;
						}
						keyDelay++;
					}
					if(wasdown)
						Released = true;
					Down = false;
					Pressed = false;
				}
			}

			void Debug(String^ text)
			{
				Console::WriteLine(text);
				cout << " :Key Code: "<<keyCode << endl;
				cout << " :Action Code: " << keyActionCode << endl;
				cout << " :Pressed: "<<Pressed << endl;
				cout << " :Down: " << Down<< endl;
				cout << " :Released: " << Released << endl;
				cout << " :Up: " << Up << endl;
				cout << " :key delay: " << keyDelay << endl;
			}

			int keyCode;
			int keyActionCode;
			bool Pressed;
			bool Down;
			bool Up;
			bool Released;
			int keyDelay;

		private: 
			bool wasdown;
		};

		public ref class Keyboard : Input
		{
		public:

			static Keyboard^ defaultKeyboard;

			void HookEvents()
			{
				if (Core::CoreEvents == nullptr)
					return;
				Core::CoreEvents->OnKeyboardEvent += gcnew KeyboardEventHandler(this, &Keyboard::On_Keyboard);
			}

			//function to call in an event.  Overrride to use
			virtual void On_Keyboard(int KeyCode, int ActionCode, int ScanCode, int ModeCode)
			{
				ReadKeyboard(KeyCode, ActionCode, ScanCode, ModeCode);
				OnKeyboard(KeyCode, ActionCode, ScanCode, ModeCode);
			}

			virtual void OnKeyboard(int k, int a, int s, int m)
			{

			}

			KeyButton Key;
			ButtonAction Action;
			int keyDelay;
			bool KeyDown;
			bool KeyUp;
			bool KeyPressed;
			bool KeyReleased;
			bool debugMode;
			int keyCode;
			int actionCode;
			int scanCode;
			int modCode;
			int _keycode;
			int _delay;

			Keyboard()
			{
			}

			bool IsPressed(KeyButton button)
			{
				if (keyDelay > 15)
				{
					if (KeyPressed && Key == button)
					{
						keyDelay = 0;
						return true;
					}
				}
				return false;
			}

			bool IsDown(KeyButton button)
			{
				if (keyDelay > 3)
				{
					if (KeyDown && Key == button)
					{
						keyDelay = 0;
						return true;
					}
				}
				return false;
			}

			//make static and call from gxWindow
			void ReadKeyboard(int key, int action, int scancode, int mods)
			{
				_keycode = key;
				if (actionCode == 3)
					actionCode = action;

				if (action == 0 && (actionCode == 2 || actionCode == 1))
					actionCode = 3;
				if(actionCode != 3)
					actionCode = action;
				
				switch (key)
				{
				case 81:
					Key = KeyButton::Q;
					break;
				case 87:
					Key = KeyButton::W;
					break;
				case 69:
					Key = KeyButton::E;
					break;
				case 82:
					Key = KeyButton::R;
					break;
				case 84:
					Key = KeyButton::T;
					break;
				case 89:
					Key = KeyButton::Y;
					break;
				case 85:
					Key = KeyButton::U;
					break;
				case 73:
					Key = KeyButton::I;
					break;
				case 79:
					Key = KeyButton::O;
					break;
				case 80:
				{
					Key = KeyButton::P;
					break;
				}
				case 65:
					Key = KeyButton::A;
					break;
				case 83:
					Key = KeyButton::S;
					break;
				case 68:
					Key = KeyButton::D;
					break;
				case 70:
					Key = KeyButton::F;
					break;
				case 71:
					Key = KeyButton::G;
					break;
				case 72:
					Key = KeyButton::H;
					break;
				case 74:
					Key = KeyButton::J;
					break;
				case 75:
					Key = KeyButton::K;
					break;
				case 76:
					Key = KeyButton::L;
					break;
				case 90:
					Key = KeyButton::Z;
					break;
				case 88:
					Key = KeyButton::X;
					break;
				case 67:
					Key = KeyButton::C;
					break;
				case 86:
					Key = KeyButton::V;
					break;
				case 66:
					Key = KeyButton::B;
					break;
				case 78:
					Key = KeyButton::N;
					break;
				case 77:
					Key = KeyButton::M;
					break;
				case 257:
					Key = KeyButton::Enter;
					break;
				case 265:
					Key = KeyButton::Up;
					break;
				case 264:
					Key = KeyButton::Down;
					break;
				case 263:
					Key = KeyButton::Left;
					break;
				case 262:
					Key = KeyButton::Right;
					break;
				case 48:
					Key = KeyButton::n0;
					break;
				case 49:
					Key = KeyButton::n1;
					break;
				case 50:
					Key = KeyButton::n2;
					break;
				case 51:
					Key = KeyButton::n3;
					break;
				case 52:
					Key = KeyButton::n4;
					break;
				case 53:
					Key = KeyButton::n5;
					break;
				case 54:
					Key = KeyButton::n6;
					break;
				case 55:
					Key = KeyButton::n7;
					break;
				case 56:
					Key = KeyButton::n8;
					break;
				case 57:
					Key = KeyButton::n9;
					break;
				case 258:
					Key = KeyButton::Tab;
				case 333:
					Key = KeyButton::Plus;
				case 334:
					Key = KeyButton::Minus;
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
						KeyReleased = true;
						KeyDown = false;
						KeyUp = true;
					}

					if (KeyReleased)
					{
						Action = ButtonAction::Up;
						KeyReleased = false;
						key = 0;
						Key = KeyButton::Null;
						_keycode = 0;
					}

					break;
				case 1:
					Action = ButtonAction::Pressed;
					KeyPressed = true;
					KeyDown = true;
					KeyUp = false;
					_delay = 0;
					break;
				case 2:
					Action = ButtonAction::Down;
					KeyUp = false;
					KeyPressed = false;
					KeyDown = true;
					_delay = 0;
					break;
				case 3:
					if (debugMode)
						cout << "\n-> Mouse Input class -> Forth mouse button state" << endl;
					break;
				default:
					break;
				}

				_delay++;
				//keyDelay++;
				if (debugMode)
				{
					Debug();
				}
			}

			void Keyboard::Debug()
			{
				Debug("Keyboard: ");
			}

			void Keyboard::Debug(String^ text)
			{
				Console::WriteLine(text);
				Console::Write(Action.ToString() + ".");
				Console::Write(Key.ToString() + ".");
				Console::WriteLine("num: " + _keycode.ToString());
			}
		};
	}
}