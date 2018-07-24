#pragma once
#include "Input.h"
#include "Core.h"
#include "Vector.h"

namespace Antimatter {
	namespace Games {


		public ref class Mouse
		{
		private:

			bool isDown;
			bool isUp;
			static int MiceCount=0;
			void Construction()
			{
				double x = 0.0;
				double y = 0.0;
				int scroll = 0;
				int buttonCode = 0;
				int actionCode = 0;
				int modCode = 0;
				int deltaX = 0;
				int deltaY = 0;
				int zoom = 0;
				position = gcnew Vector();
				MouseButton button = MouseButton::None;
				MouseAction action = MouseAction::Up;
				isDown = false;
				MiceCount++;
				HookEvents();
				
			}

		public:
			Mouse()
			{
				Construction();
			}
			~Mouse()
			{
				UnHookEvents();
			};
			!Mouse()
			{
			};

			double x;
			double y;
			int scroll;
			int buttonCode;
			int actionCode;
			int modCode;
			int deltaX;
			int deltaY;
			int zoom;
			Vector^ position;
			MouseButton button;
			MouseAction action;
			bool IsDown(MouseButton mb);
			bool IsReleased(MouseButton mb);
			bool IsPressed(MouseButton mb);
			bool IsUp(MouseButton mb);

			static Mouse^ Default;
			static property Vector^ WindowsFormPosition
			{
				Vector^ get()
				{
					return gcnew Vector((float)Control::MousePosition.X, (float)Control::MousePosition.Y, 0.f);
				}
			};

			virtual void OnClick(int MouseButtonCode, int MouseActionCode, int ModCode)
			{
				action = (MouseAction)MouseActionCode;
				button = (MouseButton)MouseButtonCode;
				buttonCode = MouseButtonCode;
				actionCode = MouseActionCode;
				modCode = ModCode;
			}

			virtual void OnMove(double X, double Y)
			{
				x = X;
				y = Y;
				position->x = (float)X;
				position->y = (float)Y;
			}

			virtual void OnScroll(int deltax, int deltay)
			{
				deltaX = deltax;
				deltaY = deltay;
				scroll = deltay;
				zoom += deltay;
				position->z += (float)deltay;
			}

			void HookEvents()
			{
				if (Core::CoreEvents == nullptr)
					return;

				Core::CoreEvents->OnMoveEvent += gcnew MousePositionEventHandler(this, &Mouse::OnMove);
				Core::CoreEvents->OnMouseClickEvent += gcnew MouseClickEventHandler(this, &Mouse::OnClick);
				Core::CoreEvents->OnScrollEvent += gcnew MouseScrollEventHandler(this, &Mouse::OnScroll);
			}

			void UnHookEvents()
			{
				if (Core::CoreEvents == nullptr)
					return;

				Core::CoreEvents->OnMoveEvent -= gcnew MousePositionEventHandler(this, &Mouse::OnMove);
				Core::CoreEvents->OnMouseClickEvent -= gcnew MouseClickEventHandler(this, &Mouse::OnClick);
				Core::CoreEvents->OnScrollEvent -= gcnew MouseScrollEventHandler(this, &Mouse::OnScroll);
			}

			Vector^ GetNormalisedPostion()
			{
				//check boxing please
				float normalizedX = -1.0f + 2.0f * (float)x /(float) glx::Core::windowWidth;
				float normalizedY = 1.0f - 2.0f * (float)y / (float)glx::Core::windowHeight;
				return gcnew Vector(normalizedX, normalizedY, 0.f);
			}
		};
	}
}

