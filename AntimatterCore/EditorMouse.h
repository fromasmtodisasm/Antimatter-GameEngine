#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Camera.h"
#include "Input.h"
#include "Mouse.h"

//http://stackoverflow.com/questions/4942311/how-do-i-assign-an-event-handler-to-an-event-in-c-cli

namespace Antimatter {
	namespace Games {

		public ref class EditorMouse
		{
		private:

			Vector^ oldMousePan = gcnew Vector();
			Vector^ oldMouseSpin = gcnew Vector();
			bool togglex = false;
			bool toggley = false;
			Vector^ oldMousePos = gcnew Vector();
			Vector^ point = gcnew Vector(0, 5, 0);
			Vector^ mouseRot = gcnew Vector(0, 0, 0);
			double mouseX = 0;
			double mouseY = 0;

		public:
			int buttonCode;
			int actionCode;
			int modCode;
			double deltaX;
			double deltaY;
			double scroll;
			bool debug;
			bool buttonDown;
			bool toggle;
			//Vector^ position;
			Mouse^ mouse;
			static EditorMouse^ Default;
			static int zoom;

			EditorMouse()
			{
				mouse = Mouse::Default;
//				HookEvents();
				//position = gcnew Vector();
				//SetState(MouseState::GlobalState);
			};

			~EditorMouse()
			{
		//		UnHookEvents();
			};
			!EditorMouse()
			{
			};

			
			/*
			void SetState(MouseState^ m)
			{
				x = m->x;
				y = m->y;
				//position = gcnew Vector((float)x,(float)y);
				scroll = m->scroll;
				button = m->button;
			};
			*/

			Vector^ ToggleInputToVector(Vector^ newPos, Vector^ oldPos, float panSpeed, float thresh, float Ythresh)
			{
				Vector^ delta = newPos - oldPos;
				Vector^ output = gcnew Vector();
				if (delta->x < -thresh || delta->x > thresh)
				{

					if (newPos->x < oldPos->x)
						output -= gcnew Vector(panSpeed * -delta->x, 0, 0);

					if (newPos->x > oldPos->x)
						output += gcnew Vector(panSpeed * delta->x, 0, 0);

					togglex = true;
				}
				if (delta->y < -Ythresh - Ythresh || delta->y > thresh + Ythresh)
				{
					
					if (newPos->y < oldPos->y)
						output -= gcnew Vector(0, panSpeed *-delta->y, 0);

					if (newPos->y > oldPos->y)
						output += gcnew Vector(0, panSpeed * delta->y, 0);

					toggley = true;
				}
				return output;
			}

			void Control(Camera^ camera)
			{
				if (Control::MouseButtons == MouseButtons::Right)
				{
					Vector^ dir = gcnew Vector();
					float mult = 0.1f;
					Vector^ mousePos = gcnew Vector((float)mouse->x, (float)mouse->y, 0.f);
					float panSpeed = 0.009f;
					float thresh = 0.01f;
					float Ythresh = 0.2f;
					dir = camera->eye->ForwardDirection * mult;
					Vector^ input = ToggleInputToVector(mousePos, oldMousePan, panSpeed, thresh, Ythresh);
					Vector^ v = gcnew Vector();

					if (togglex)
						v += camera->target->RightDirection * (input->x*-mult);
					if (toggley)
						v += camera->target->ForwardDirection * (input->y*-mult);

					camera->eye->Position += v;
				}
				else {
					oldMousePan = gcnew Vector((float)mouse->x, (float)mouse->y, 0.f);
					togglex = false;
					toggley = false;
				}

				if (Control::MouseButtons == MouseButtons::Middle)
				{
					Vector^ mousePos = gcnew Vector((float)mouse->x, (float)mouse->y, 0.f);
					float panSpeed = 0.00045f;
					float thresh = 1.0f;
					float Ythresh = 0.2f;

					Vector^ orientation = InputToEulerAngles(mousePos, oldMouseSpin, panSpeed, thresh, Ythresh);
					camera->eye->EulerAngles += gcnew Vector(orientation->x*3.f, orientation->y * 4.f, 0.f);
				}
				else {
					oldMouseSpin = gcnew Vector((float)mouse->x, (float)mouse->y, 0.f);
				}
				camera->eye->Position += gcnew Vector(0.f, ((float)mouse->scroll *0.1), 0.f);
				mouse->scroll = 0.f;
			}

			Vector^ InputToEulerAngles(Vector^ newPos, Vector^ oldPos, float panSpeed, float thresh, float Ythresh)
			{
				Vector^ delta = newPos - oldPos;
				Vector^ output = gcnew Vector();
				if (delta->x < -thresh || delta->x > thresh)
				{
					if (newPos->x < oldPos->x)
						output -= gcnew Vector(0, panSpeed * delta->x, 0);

					if (newPos->x > oldPos->x)
						output += gcnew Vector(0, panSpeed * -delta->x, 0);
				}
				if (delta->y < -Ythresh - Ythresh || delta->y > thresh + Ythresh)
				{

					if (newPos->y < oldPos->y)
						output -= gcnew Vector(panSpeed * -delta->y, 0, 0);

					if (newPos->y > oldPos->y)
						output += gcnew Vector(panSpeed * delta->y, 0, 0);
				}
				return output;
			}
		};
	}
}
