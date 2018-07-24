#pragma once
#include "GlaxionHeader.h"
#include "glaxCore.h"
#include "Vector.h"
#include "glaxRenderer.h"
#include "Core.h"
#include "tool.h"

////http://www.codeproject.com/Articles/16051/Creating-an-OpenGL-view-on-a-Windows-Form

namespace Antimatter
{
	namespace Games
	{
		public ref class MainWindow
		{
		public:
			MainWindow()
			{
				gxCore = glx::Core::GetCore();
				HookEvents();
			//	gxWindow = new glx::Window();
			//	gxWindow->Initialize();
			};

			~MainWindow()
			{
				gxCore = NULL;
				delete gxCore;
				UnHookEvents();
			};

			!MainWindow()
			{
			};
			
			glx::Core* gxCore;
			Vector^ MousePosition = gcnew Vector();
			int width;
			int height;
			double MouseDeltaX;
			double MouseDeltaY;
			List<String^>^ DropFiles = gcnew List<String^>();
			bool Initialized;
			String^ Title;
			static MainWindow^ mainWindow;
		
			void HookEvents()
			{
				if (mainWindow != nullptr)
				{
					Core::CoreEvents->ResizeEvent += gcnew ResizeEventHandler(this, &MainWindow::OnResize);
					Core::CoreEvents->FileDropEvent += gcnew DropEventHandler(this, &MainWindow::OnFileDrop);
				}
			}

			void UnHookEvents()
			{
				if (mainWindow != nullptr)
				{
					Core::CoreEvents->ResizeEvent -= gcnew ResizeEventHandler(this, &MainWindow::OnResize);
					Core::CoreEvents->FileDropEvent -= gcnew DropEventHandler(this, &MainWindow::OnFileDrop);
				}
			}

			void SetWindowTitle(String^ text)
			{
				Title = text;
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				const char* str = static_cast<const char*>(p.ToPointer());
				gxCore->SetWindowTitle(str);
				Marshal::FreeHGlobal(p);
			}

			virtual void OnResize(int Width, int Height)
			{
				width = Width;
				height = Height;
				tool::debug("Window:: width: ", Width, "height: ", Height);
			
			}

			virtual void OnFileDrop(List<String^>^ files)
			{

			}


			//implement dispose
			
		};
	}
}

