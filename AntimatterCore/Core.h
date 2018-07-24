#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include <msclr\marshal_cppstd.h>
//#include "MouseState.h"
//#include "tool.h"

using namespace System::Runtime::InteropServices;

//clr memory managment
//https://www.codeproject.com/articles/1139538/cplusplus-cli-under-the-hood#Memory

//double thunking and managed/native calling types
//https://msdn.microsoft.com/en-us/library/ms235292.aspx?f=255&MSPPError=-2147217396

//debugg
//__funcsig__
//https://voidnish.wordpress.com/2005/11/12/the-__funcsig__-macro/
namespace Antimatter
{
	namespace Games
	{
		//system stuff
		public delegate void UpdateEventHandler(float);
		//public delegate void UpdateEventHandler();
		public delegate void ErrorEventHandler(String^);
		public delegate void ResizeEventHandler(int, int);
		public delegate void DropEventHandler(List<String^>^);

		//mouse
		public delegate void MouseClickEventHandler(int, int, int);
		//public delegate void MouseEventHandler(MouseState^);
		public delegate void MousePositionEventHandler(double, double);
		public delegate void MouseScrollEventHandler(int, int);

		//keyboard
		public delegate void KeyboardEventHandler(int, int, int, int);

		public ref class Core
		{
		public:

			Core()
			{

			};

			static Core^ CoreEvents = gcnew Core();
			static bool Assert;
			static float DeltaTime;
			static Int64 DeltaTimeInt;
			static Int64 frame;

			event UpdateEventHandler^ OnUpdateEvent;
			event ErrorEventHandler^ OnErrorEvent;

			//does an instanced class need these?  Will they work in it's current state?
			event MouseClickEventHandler^ OnMouseClickEvent;   // declare the event OnClick  
			event MousePositionEventHandler^ OnMoveEvent;   // declare OnDblClick 
			event MouseScrollEventHandler^ OnScrollEvent;
			event MouseEventHandler^ OnMouseEvent;
			event KeyboardEventHandler^ OnKeyboardEvent;

			static void HookEvents(Core^ core)
			{
				if (core == nullptr)
					return;
				core->OnUpdateEvent += gcnew UpdateEventHandler(core, &Core::OnUpdate);
				core->OnErrorEvent += gcnew ErrorEventHandler(core, &Core::OnError);
				core->ResizeEvent += gcnew ResizeEventHandler(core, &Core::OnResize);
				core->FileDropEvent += gcnew DropEventHandler(core, &Core::OnFileDrop);
			}

			static void UnhookEvents(Core^ core)
			{
				if (core == nullptr)
					return;
				core->OnUpdateEvent -= gcnew UpdateEventHandler(core, &Core::OnUpdate);
				core->OnErrorEvent -= gcnew ErrorEventHandler(core, &Core::OnError);
				core->ResizeEvent -= gcnew ResizeEventHandler(core, &Core::OnResize);
				core->FileDropEvent -= gcnew DropEventHandler(core, &Core::OnFileDrop);
			}
			
			//called from the native c++ window, raises event and passes data to cli
			static void Keyboard_RaiseEvents(int button, int action, int scanCode, int mods)
			{
				CoreEvents->OnKeyboardEvent(button, action, scanCode, mods);
			}

			//dummy event, subscribe to call functionality
			virtual void OnKeyboard(int KeyCode, int ActionCode, int ScanCode, int ModeCode)
			{

			}

			static void MouseButton_CallBack(int button, int action, int mods);

			static void MousePosition_CallBack(double x, double y);

			static void MouseScroll_CallBack(int deltax, int deltay);

			static glm::vec2 GetNDC2D(float x, float y);

			static void Break(int count)
			{
				int b = count;
				int i = 0;

				ConsoleColor oldccb = Console::BackgroundColor;
				ConsoleColor oldccf = Console::ForegroundColor;
				Console::BackgroundColor = ConsoleColor::DarkYellow;
				Console::ForegroundColor = ConsoleColor::Gray;
				Console::WriteLine("Core break");
				Console::BackgroundColor = oldccb;
				Console::ForegroundColor = oldccf;

				while (i<10000000*count)
				{
					i++;
					//Console::Write(i);
				}
			}

			static void Update_CallBack(float Frame)
			{
				CoreEvents->OnUpdateEvent(Frame);
			}

			static void Error_CallBack(const char* description)
			{
				Core::CoreEvents->OnError(gcnew String(description));
			}

			virtual void OnUpdate(float Frame)
			{

				//if (debug)
				//	tool::debug("Core::Update: ", Frame);
			}


			//need to be virtual?
			virtual void OnClick(int button, int action, int mods)
			{
			}

			virtual void OnMove(double x, double y)
			{
			}

			virtual void OnScroll(int deltax, int deltay)
			{
			}

			virtual void OnError(String^ error)
			{
				//if (debug)
				//	tool::debug("Core::Error: ", error);
			}

			event ResizeEventHandler^ ResizeEvent;
			event DropEventHandler^ FileDropEvent;


			virtual void OnResize(int Width, int Height)
			{
				//tool::debug("Window:: width: ", Width, "height: ", Height);
				if (debug)
				{
					//tool::debug("Window:: width: ", Width, "height: ", Height);
				}
			}

			virtual void OnFileDrop(List<String^>^ files)
			{

			}

			static void Resize_CallBack(int Width, int Height)
			{
				CoreEvents->OnResize(Width, Height);
			}

			static List<String^>^ DropFiles = gcnew List<String^>();

			static void Drop_CallBack(int count, const char** files)
			{
				DropFiles->Clear();
				for (int i = 0; i < count; i++)
				{
					cerr << files[i];
					const char* path = files[i];

					DropFiles->Add(gcnew String(path));
					//engine->DropCallBack(gcnew String(path));
				}
				
				//convert to char** to list
				CoreEvents->OnFileDrop(DropFiles);
			}

			bool debug;
		};
	}
}

