#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Mesh.h"
#include "Renderer.h"
#include "EditorMouse.h"
#include "Keyboard.h"
#include "Component.h"

namespace Antimatter {
	namespace Games {

		public ref class Scene : Component
		{
		private:
			void Constructor()
			{
			   gameObjects = gcnew List<Mesh^>();
			   BackgroundColour = gcnew Colour(0.0001f, 0.001f, 0.02f, 0.01f);
			   camera = gcnew Camera();
			   cameraIndex = 0;
			   editorMouse = EditorMouse::Default;
			   name = "Test Scene";
			   drawSprites = true;
			   renderer = Render::current;
			   keyboard = Keyboard::defaultKeyboard;
			}

		public:

			String^ name;
			List<Mesh^>^ gameObjects;
			List<Camera^>^ cameras = gcnew List<Camera^>();
			List<Sprite^>^ sprites = gcnew List<Sprite^>();
			int cameraIndex;
			Render^ renderer;
			Camera^ camera;
			Colour^ BackgroundColour;
			Thing^ selectedObject;
			bool DrawAABBs;
			bool drawSprites;
			bool clearLines;
			bool ForceCurrentContext;
			bool debug;
			Mesh^ block;
			EditorMouse^ editorMouse;
			Keyboard^ keyboard;
			static Scene^ Current = gcnew Scene();

			Scene()
			{
				Constructor();
			};

			~Scene() {};
			!Scene() {};

			Scene(String^ Name)
			{
				name = Name;
				Constructor();
			};

			Scene(Render^ renderTarget)
			{
				Constructor();
				renderer = renderTarget;
			}

			Scene(String^ Name, Render^ renderTarget)
			{
				Constructor();
				renderer = renderTarget;
				name = Name;
			}

			Mesh^ AddMesh(String^ path)
			{
				Mesh^ m = gcnew Mesh(path);
				gameObjects->Add(m);
				return m;
			}

			Sprite^ AddSprite(String^ path)
			{
				Sprite^ m = gcnew Sprite(path);
				sprites->Add(m);
				return m;
			}

			Mesh^ CreateMesh(String^ filePath)
			{
				Mesh^ go = gcnew Mesh(filePath);
				gameObjects->Add(go);
				return go;
			}

			static void MakeCurrent(Scene^ scene)
			{
				Scene::Current = scene;
				tool::debugDarkGreen(scene->name + " is current scene");
			}

			void Scene::RenderSelection()
			{
				//ObjectSelection::ClearIDs();
				glClearColor(1, 1, 1, 1);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glx::Render::EnableTransparency(false);
				for each(Mesh^ go in gameObjects)
				{
					if (go->selectable)
					{
						//go->GenerateID();
						glx::RenderSetting s;
						s.lightingEnabled = false;
						s.solid = true;
						glx::Render::Default->DrawMesh(*go->shape->vao, go->gxtransform->GetWorldMatrix(), ObjectSelection::colourIDs[go->selectionID]->glm(), s, 0);
					}
				}

				for each(Sprite^ sprite in sprites)
				{
					if (sprite->selectable)
					{
						//sprite->GenerateID();
						glx::RenderSetting s;
						s.lightingEnabled = false;
						s._selectable = true;
						s.disableCulling = sprite->renderSettings->disableCulling;
						glx::Render::Default->DrawSprite(*sprite->vao, sprite->gxtransform->GetWorldMatrix(), ObjectSelection::colourIDs[sprite->selectionID]->glm(), 0, s, sprite->billboard);
					}
				}
				glx::Render::EnableTransparency(true);
			}

			Mesh^ Scene::SelectThing(double x, double y,bool debug)
			{
				int id = SelectByID(x, y, debug);
				for each(Mesh^ t in gameObjects)
				{
					int objectID = t->selectionID;
					bool iscolour = (id == objectID);
					if (iscolour && t->selectable)
					{
						selectedObject = t;
						return t;
					}
				}
				return nullptr;
			}

			int Scene::SelectByID(double x, double y, bool debug)
			{
				glDisable(GL_FRAMEBUFFER_SRGB);
				RenderSelection();
				int ID = glx::Render::ReadSelectionID(x, y);
				glEnable(GL_FRAMEBUFFER_SRGB);
				return ID;
			}

			void Scene::DrawThings()
			{
				PrepareScene();
				if(ForceCurrentContext)
					renderer->gxCore->MakeCurrentContext();

				for each(Mesh^ go in gameObjects)
				{
					if (go->visible)
					{
						if (DrawAABBs)
							go->renderSettings->drawAABB = true;
						go->Draw();
					}
				}
				
				if (drawSprites)
				{
					for each(Sprite^ s in sprites)
					{
						s->Draw();
					}
				}
			}

			void PrepareScene()
			{
				editorMouse->Control(camera);
				renderer->ComputeView(camera);
				glm::vec4 bc = BackgroundColour->glm();
				glClearColor(bc.x, bc.y, bc.z, bc.w);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			void RunUpdate()
			{
				cout << "scene run update" << endl;
				Update();
				DrawThings();
			}
		};
	}
}

