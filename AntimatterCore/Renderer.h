#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "glaxRenderer.h"
#include "glaxCore.h"
#include "Transform.h"
#include "Colour.h"
#include "Camera.h"
#include "MainWindow.h"
#include "LineGraphic.h"
#include "XFileLoader.h"
#include "Shape.h"
#include "glaxRender2D.h"
#include "Sprite.h"
#include "RenderSettings.h"
#include "Transform2D.h"
#include "Collider2d.h"
#include "Core.h"

//billboards
//http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

//sort order
//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-10-transparency/

//http://webglfactory.blogspot.co.uk/2011/05/how-to-convert-world-to-screen.html

namespace Antimatter {
	namespace Games {

		public ref class Render
		{
		private:
			static Render^ _current;

		public:
			Render()
			{
				gxRenderer = glx::Render::GetRenderer();
				gxCore = glx::Core::GetCore();
				window = gcnew MainWindow();
				//generate default sprite for the native renderer
				spriteShape = Shape::MeshFromXFile("Assets//sprite.x");
				glx::Render::DefaultSprite = spriteShape->vao;
				gxRenderer->worldVbo = *spriteShape->vao;
			};

			Shape^ spriteShape;
			float movex;
			MainWindow^ window;
			glx::Render* gxRenderer;
			glx::Core* gxCore;
			glm::vec4* BackColour;
			List<unsigned int>^ textures = gcnew List<unsigned int>();
			
			static property Render^ current
			{
				Render^ get()
				{
					return GetCurrentRender();
				}
				void set(Render^ value)
				{
					SetCurrentRender(value);
				}
			}
			static Render^ GetCurrentRender()
			{
				if (_current == nullptr)
					_current = gcnew Render();
				return _current;
			}
			static void SetCurrentRender(Render^ r)
			{
				if (_current == r)
					return;
				_current = r;
			}

			void InitializeShaders()
			{
				glx::Render::Default->InitializeShader();
			}

			static Sprite^ AddSpriteToRender(String^ path)
			{
				Sprite^ s = gcnew Sprite(path);
				if (s == nullptr)
					return nullptr;
				s->renderSettings->lightingEnabled = false;
				s->renderSettings->onTop = true;
				s->billboard = true;
				return s;
			}

			static void UpdateStaticSprite(Sprite^% s, String^ path)
			{
				if (s == nullptr)
					s = AddSpriteToRender(path);
			}

			static void DrawFace(Face^ f)
			{
				Render::DrawDebugLine(f->vertexA->pos, f->vertexB->pos, Colour::White);
				Render::DrawDebugLine(f->vertexB->pos, f->vertexC->pos, Colour::White);
				Render::DrawDebugLine(f->vertexC->pos, f->vertexA->pos, Colour::White);
			}

			static void DrawFace(Face^ f,Transform^ t)
			{
				Render::DrawDebugLine(t*f->vertexA->pos->W(1.f), t*f->vertexB->pos->W(1.f), Colour::White);
				Render::DrawDebugLine(t*f->vertexB->pos->W(1.f), t*f->vertexC->pos->W(1.f), Colour::White);
				Render::DrawDebugLine(t*f->vertexC->pos->W(1.f), t*f->vertexA->pos->W(1.f), Colour::White);
			}

			property glm::mat4* MVP
			{
				glm::mat4* get()
				{
					return &gxRenderer->MVP;
				}
			};

			void Render::SetProjection(float fov, int width, int height, float nearview, float farview)
			{
				gxRenderer->SetPerspective(fov, width, height, nearview, farview);
			}

			Vector^ GetNDC(Vector^ vec)
			{
				glm::vec4 v = gxRenderer->MVP*vec->ToGLM4(1);
				return gcnew Vector(v.x / v.w, v.y / v.w, v.z / v.w);
			}

			static void DebugVector(String^ text, Vector^ vector, Colour^ colour, float scale)
			{
				vector->Display(text,scale);
			}

			static void DrawVector(Vector^ a, Vector^ b)
			{
				DrawDebugLine(a, a + b, Colour::New);
			}
			static void DrawVector(Vector^ a, Vector^ b, Colour^ col)
			{
				DrawDebugLine(a, a + b, col);
			}

			static void DrawText(String^ text, float x, float y, Colour^ colour, float scaleX,float scaleY)
			{
				FontManager::current->RenderString(text, x, y, colour, scaleX, scaleY);
			}

			static void DrawText(String^ text, float y, Colour^ colour,float scale)
			{
				FontManager::current->RenderString(text, 0, y, colour, scale,scale);
			}

			static void DrawText(String^ text, float y, Colour^ colour)
			{
				FontManager::current->RenderString(text, 0, y, colour, 0.5f,0.5f);
			}

			static void DrawText(String^ text, float y)
			{
				FontManager::current->RenderString(text, 0, y, Colour::White, 0.5f, 0.5f);
			}

			static void DebugText(String^ text, float y)
			{
				FontManager::current->RenderString(text, 0, y, Colour::Green, 1,1);
			}

			static void DebugText(float y,...cli::array<String^>^ text)
			{
				for(int i=0;i<text->Length;i++)
				FontManager::current->RenderString(text[i], 0, y+(i*50), Colour::Green, 1, 1);
			}

			static void Render::RenderPoint(Vector^ point, Colour^ col, Transform^ trans, float size, bool ontop)
			{
				glm::quat q = trans->gxtransform->WorldRotation;
				glm::vec3 up = q*glm::vec3(0, size, 0);
				glm::vec3 right = q*glm::vec3(size, 0, 0);
				glm::vec3 forward = q*glm::vec3(0, 0, size);
				glm::vec3 p = point->glm();
				glm::vec4 c = col->glm();

				glx::Render::Default->DrawLine(p - up, p + up, c, c, size, true, false);
				glx::Render::Default->DrawLine(p - right, p + right, c, c, size, true, false);
				glx::Render::Default->DrawLine(p - forward, p + forward, c, c, size, true, false);
			}

			static void Render::DrawPoint(Vector^ point, Colour^ col, float size)
			{
				Vector^ up = gcnew Vector(0, size, 0);
				Vector^ right = gcnew Vector(size, 0, 0);
				Vector^ forward = gcnew Vector(0, 0, size);

				Render::DrawDebugLine(point, point + up, col);
				Render::DrawDebugLine(point, point - up, col);
				Render::DrawDebugLine(point, point + right, col);
				Render::DrawDebugLine(point, point - right, col);
				Render::DrawDebugLine(point, point + forward, col);
				Render::DrawDebugLine(point, point - forward, col);
			}
			static void Render::DrawPoint(Vector^ point, Colour^ col)
			{
				float size = 0.5f;
				Vector^ up = gcnew Vector(0, size, 0);
				Vector^ right = gcnew Vector(size, 0, 0);
				Vector^ forward = gcnew Vector(0, 0, size);

				Render::DrawDebugLine(point, point + up, col);
				Render::DrawDebugLine(point, point - up, col);
				Render::DrawDebugLine(point, point + right, col);
				Render::DrawDebugLine(point, point - right, col);
				Render::DrawDebugLine(point, point + forward, col);
				Render::DrawDebugLine(point, point - forward, col);
			}

			void Render::DrawLink(Vector^ a, Vector^ b, Colour^ colour, bool onTop)
			{
				Vector^ junction = gcnew Vector(b->x, a->y, b->z);
				DrawLine(a, junction, colour, onTop);
				DrawLine(junction, b, colour, onTop);
			};
		
			static Vector^ Get2DPoint(Vector^ point)
			{
				glm::vec2 v = glx::Render::Default->Get2dPoint(point->glm());
				return gcnew Vector(v.x,v.y,0);
			}

			static Vector^ Get3DPoint(Vector^ point)
			{
				glm::vec3 v = glx::Render::Default->Get3dPoint(point->ToGLM2());
				return gcnew Vector(v);
			}

			static Vector^ Get3DPoint(float x, float y)
			{
				glm::vec3 v = glx::Render::Default->Get3dPoint(glm::vec2(x,y));
				return gcnew Vector(v);
			}

			static Vector^ Get3DPoint(Camera^ camera,float x, float y,float distance)
			{
				glm::mat4 proj = Render::current->gxRenderer->projectionMatrix;
				glm::mat4 view = Render::current->gxRenderer->viewMatrix;
				glm::mat4 model = camera->eye->gxtransform->staticMatrix;
				glm::vec4 viewPort = glm::vec4(0, 0, glx::Core::windowWidth, glx::Core::windowHeight);
				glm::vec3 result = glm::unProject(glm::vec3(x,y,distance), model, proj,viewPort);
				
				return gcnew Vector(result);
			}

			static void ScreenToWorldRay(float x, float y)
			{
				glm::vec3 ray = glx::Render::Default->CreateRay(x, y);
				gcnew Vector(ray);
			}
			
			bool GetDebugScene()
			{
				return gxRenderer->debugScene;
			}

			static Colour^ ReadPixel(double x, double y)
			{
				double lx = x;
				double ly = y;
				Colour^ col = gcnew Colour(glx::Render::ReadPixel(lx, ly));
				
				return col;
			}

			static Colour^ ReadColour(double x, double y)
			{
				double lx = x;
				double ly = y;
				Colour^ col = gcnew Colour(glx::Render::ReadColour(lx, ly));
				
				return col;
			}

			void SetDebugScene(bool toggle)
			{
				gxRenderer->debugScene = toggle;
				Console::WriteLine("Renderer: gxRenderer -> drawScene: " + GetDebugScene());
			}

			static Vector^ CreateMouseRay(float x, float y)
			{
				glm::vec3 dir = glx::Render::Default->CreateRay(x, y);

				return gcnew Vector(dir);
			}

			static void DrawAxis(Transform^ transform)
			{
				DrawLine(transform->Position, transform->Forward, gcnew Colour(0, 0, 1));
				DrawLine(transform->Position, transform->Up, gcnew Colour(1, 1, 0));
				DrawLine(transform->Position, transform->Right, gcnew Colour(1, 0, 0));
			}

			static void DrawDebugAxis(Transform^ transform)
			{
				DrawLine(transform->Position, transform->Forward, gcnew Colour(0, 0, 1),true);
				DrawLine(transform->Position, transform->Up, gcnew Colour(1, 1, 0),true);
				DrawLine(transform->Position, transform->Right, gcnew Colour(1, 0, 0),true);
			}

			void GetOpenGLError()
			{
				GLenum err;
				while ((err = glGetError()) != GL_NO_ERROR)
				{
					std::cout << "openGL ERROR!: "<<err << endl;
					//requires freeglut
					//std::cout << "glut error string: " << gluErrorString(err) << endl;
				}
			}

			static void DrawLine(Vector^ point1, Vector^ point2, Colour^ colour,bool onTop)
			{
				gcnew LineGraphic(point1, point2, colour, onTop); 
			}

			static void DrawLine(Vector^ point1, Vector^ point2, Colour^ colour)
			{
				DrawLine(point1, point2, colour , false); //any garauntee mutiplr calls will add the same line?
																				//glx::Renderer::DefaultRenderer->DrawLine(point1->ToGLM(), point2->ToGLM(), &colour->ToGLX(), 5);
			}

			static void DrawDebugLine(Vector^ point1, Vector^ point2, Colour^ colour)
			{
				DrawLine(point1, point2, colour, true); 
			}

			static void RenderLine(Vector^ point1, Vector^ point2, Colour^ colour,bool drawInfront)
			{
				glx::Render::Default->DrawLine(point1->glm(), point2->glm(), colour->glm(), colour->glm(), 5,drawInfront,false);
			}

			static void RenderLine2D(Vector^ point1, Vector^ point2, Colour^ colour, bool drawInfront)
			{
				glx::Render::Default->DrawLine(point1->glm(), point2->glm(), colour->glm(), colour->glm(), 5.f, drawInfront, true);
			}

			static void RenderLine(Vector^ point1, Vector^ point2, Colour^ colour, bool drawInfront,bool orthographic)
			{
				glx::Render::Default->DrawLine(point1->glm(), point2->glm(), colour->glm(), colour->glm(),5.f, drawInfront, orthographic);
			}

			static void SetLightPosition(float X, float Y, float Z)
			{
				glx::Render::Default->SetLightPosition(X, Y, Z);
			}

			void SetLightIntensity(float R, float G, float B)
			{
				gxRenderer->SetLightIntensity(R, G, B);
			}

			void SetAmbientLight(float R, float G, float B)
			{
				BackColour = new glm::vec4(R, G, B, 1);
				gxCore->BackColour = new glm::vec4(R, G, B, 0);
				gxRenderer->SetAmbientLight(R, G, B);
			}

			void SetAttenutation(float X, float Y, float Z,float W)
			{
				gxRenderer->SetAttenuation(X, Y, Z,W);
			}

			void SetBackGroundColour(float r, float g, float b, float a)
			{
				gxRenderer->SetBackGroundColour(r, g, b, a);
			}

			void ComputeDefaultCamera()
			{
				gxRenderer->ComputeDefaultCamera();
			}

			void ComputeView(Transform^ eye, Vector^ target)
			{

				gxRenderer->ComputeView(eye->gxtransform->GetPosition(), target, eye->gxtransform->GetUpDirection());
			}

			void ComputeView(Camera^ cam)
			{
				ComputeView(cam->eye, cam->target->Position);
			}
		};
	}
}

