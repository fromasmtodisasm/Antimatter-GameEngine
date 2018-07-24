#pragma once
#include "EngineHeader.h"
#include "EngineForward.h"

using namespace System::Drawing::Imaging;
using namespace System;
using namespace System::IO;
using namespace System::IO::Compression;
using namespace System::Diagnostics;

namespace Antimatter
{
	namespace Games
	{
		public ref class Engine
		{
		private:
			void Construction();
			void InternalLoad();
		public:
			Engine();
			Engine(String^ name);
			String^ name;
			Core^ core;
			Render^ renderer;
			int Frame;
			double FPS;
			int sceneIndex;
			List<Scene^>^ scenes;
			bool debug = false;
			Camera^ defaultCamera;
			EditorMouse^ EditorMouse;
			//EditorMouse^ editorMouse;
			Keyboard^ keyboard;
			List<Controller^>^ controllers;
			List<Component^>^ components = gcnew List<Component^>();
			bool fixedUpdateStarted;
			bool runFixedUpdate;
			float elapsedTime;

			void InitializeGLEW();
			void CreateAndRun();
			virtual void Update(float);
			void RunFixedUpdate();

			virtual void Load() 
			{

			};



			void LoadComponents()
			{
				for each(Component^ c in components)
				{
					c->Load();
				}
			};
			
			static void SetMouseClickFunction(MouseClickEventHandler^ e)
			{
				Core::CoreEvents->OnMouseClickEvent += e;
			}

			static void SetMouseMoveFunction(MousePositionEventHandler^ e)
			{
				Core::CoreEvents->OnMoveEvent += e;
			}

			static void SetKeyboardFunction(KeyboardEventHandler^ e)
			{
				Core::CoreEvents->OnKeyboardEvent += e;
			}

			/*
			static void SetMouseFunction(MouseEventHandler^ e)
			{
				Core::CoreEvents->OnMouseEvent += e;
			}
			
			static void SetUpdateFunction(UpdateEventHandler^ e)
			{
				Core::CoreEvents->OnUpdateEvent += e;
			}
			*/

			virtual void SwapBuffers()
			{
				if (renderer->gxCore->glfwWindow != NULL)
				{
					glfwSwapBuffers(renderer->gxCore->glfwWindow);
				}
			}

			static void Engine::LoadGlaxionIcon(cli::array<System::Byte>^ bits,int width, int height)
			{
				pin_ptr<unsigned char> p = &bits[0];

				GLFWimage i;
				i.width = width;
				i.height = height;
				i.pixels = p;
				glfwSetWindowIcon(glx::Render::Default->gxCore->glfwWindow, 1, &i);
			}

			virtual void Close()
			{
				this->renderer->gxRenderer->Close();
				this->renderer->gxCore->Close();
			}

			virtual void DropObjects(String^ path) //load objects from the drop delegate into the engine
			{
				String^ ext = Path::GetExtension(path);
				if (ext == ".jpg" || ext == ".png" || ext == ".bmp" || ext == ".gif")
				{
					Console::WriteLine("LOADED TEXTURE");
					Sprite^ go = gcnew Sprite(path);
					Random^ rand = gcnew Random();
					go->Position = gcnew Vector((float)rand->Next(-10, 10), (float)rand->Next(0, 15), -15.f);
					currentScene->sprites->Add(go);
				}

				else if (ext == ".x")
				{
					Console::WriteLine("LOADED Mesh");
					Mesh^ go = gcnew Mesh(path);
					currentScene->gameObjects->Add(go);
				}
			}

			void OpenAssetFolder()
			{
				System::Diagnostics::Process::Start("Assets");
			}

			property Scene ^currentScene
			{
				Scene^ get() { return GetCurrentScene(); }
				void set(Scene^ InputScene) {  SetCurrentScene(InputScene); }
			};

			Scene^ GetCurrentScene()
			{
				if (scenes->Count == 0)
				{
					scenes->Add(gcnew Scene("Getted default scene", renderer));
					sceneIndex = 0;
					return scenes[0];
				}
				else
				{
					if(debug)
						cout << "Engine::Someone Called Getter on CurrentScene " << sceneIndex << endl;
					return scenes[sceneIndex];
				}	
			}

			//when setting the current scene property
			void SetCurrentScene(Scene^ InputScene)
			{
				if (scenes->Contains(InputScene))
				{
					//set the current sceneIndex to be the index of the input scene
					//possible optimisation while earching the list?
					sceneIndex = scenes->IndexOf(InputScene);
				}
				else {
					scenes->Add(InputScene);
					sceneIndex = scenes->Count - 1;
				}
			}
			void Debug()
			{
				Console::WriteLine("\nTEST Engine\n");
			}
		};
	}
}

