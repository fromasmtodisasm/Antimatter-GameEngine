#pragma once
#include"SystemHeader.h"
#include "GlaxionHeader.h"
#include <msclr/marshal.h>
#include "Colour.h"
#include "FontShader.h"
#include "glaxRenderer.h"

//screen to world?
//https://stackoverflow.com/questions/7692988/opengl-math-projecting-screen-space-to-world-space-coords

//world to screen
//https://stackoverflow.com/questions/27347021/opengl-glm-converting-a-ndc-to-world-coordinates-to-select-objects-with-mouse
using namespace msclr::interop;

namespace Antimatter {
	namespace Games
	{
		public ref class FontManager
		{
		protected:
			!FontManager()
			{
				
				freeSans->Close();
				glx::FontShader::Shutdown();
				freeSans = NULL;
				delete freeSans;
			}

		public:
			FontManager()
			{
				if (current == nullptr)
					current = this;

				freeSans = new glx::FontShader();
				freeSans->StartFontShader();
				//https://stackoverflow.com/questions/30062530/sdl-ttf-font-not-loading
				freeSans->LoadFont("Assets//FreeSans.ttf");
			}

			~FontManager()
			{
				this->!FontManager();
			}

			glx::FontShader* freeSans;
			static FontManager^ current = gcnew FontManager();

			static char* StringToChars(String^ s)
			{
				IntPtr p = Marshal::StringToHGlobalAnsi(s);
				char* str = static_cast<char*>(p.ToPointer());
				Marshal::FreeHGlobal(p);
				return str;
			}

			void DisplayVector(String ^text, glm::vec3 v,glm::vec4 colour, float scale)
			{
				glm::mat4 mat = glm::mat4(1.f);
				int row = 0;
				FontManager::current->RenderNative(text, v.x, v.y, v.z, colour, scale, scale, true, true, mat);
				FontManager::current->RenderNative("	X: " + v.x, v.x, v.y - 1, v.z, colour, scale, scale, true, true, mat);
				FontManager::current->RenderNative("	Y: " + v.y, v.x, v.y - 2, v.z, colour, scale, scale, true, true, mat);
				FontManager::current->RenderNative("	Z: " + v.z, v.x, v.y - 3, v.z, colour, scale, scale, true, true, mat);
			}

			void RenderString(String^ text, float x, float y, Colour^ colour, float scalex, float scaley)
			{
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				char* str = static_cast<char*>(p.ToPointer());
				freeSans->RenderText(str, glm::vec3(x,y,0.f), colour->glm(), scalex, scaley,glm::mat4(1.f));
				//FontManager::current->glaxFont->RenderText(str, x, y, colour->ToGLM(), scalex, scaley);
				Marshal::FreeHGlobal(p);
			}

			void DrawNDC(String^ text, float x, float y, Colour^ colour, float scaleX, float scaleY)
			{
				if (text == nullptr||text->Length == 0)
					text = "empty";
				float ndcmetric = 1000.f;
				float edget = 1.0f;
				float scalex = scaleX / ndcmetric;
				float scaley = scaleY / ndcmetric;
				if (x < -edget)
					x = -1.f;
				
				if (x > 0.f)
				{
					float length = text->Length*0.1f;
						if (x+length > edget) //probably include scale x somewhere here
						{
							//Core::Break(10);
							x = 1.f - length;
						}
				}
				
				if (y < -edget+0.1f)
					y = -0.9f;
				if (y + scaley > edget-0.2f)
					y = 0.8f - scaley;

				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				const char* str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderText(str, glm::vec3(x,y,0.f), colour->glm(), scalex, scaley,glm::mat4(1.f));
				Marshal::FreeHGlobal(p);
			}

			static void DisplayAtPoint(String ^text, glm::vec3 vec,glm::vec3 pos,float scale)
			{
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				const char* str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderText(str, pos, glm::vec4(1, 1,1, 1), scale, scale,glm::mat4(1.0f));
				//make sure length is not 0
				float xo = -text->Length / 4.f;
				float yi = scale/4.f;
				float yo = yi;
				String^ s = " X= " + vec.x;
				
				int row = 1;
				p = Marshal::StringToHGlobalAnsi(s);
				str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderText(str, pos+glm::vec3(xo, -row*yi,0), glm::vec4(1, 0.2f, 0.2f, 1), scale, scale,glm::mat4(1.0f));
				
				row++;
				s = " Y= " + vec.y;
				p = Marshal::StringToHGlobalAnsi(s);
				str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderText(str, pos + glm::vec3(xo, -row*yi,0), glm::vec4(0.2f, 1.f, 0.2f, 1.f), scale, scale,glm::mat4(1.f));
				
				row++;
				s = " Z= " + vec.z;
				p = Marshal::StringToHGlobalAnsi(s);
				str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderText(str, pos + glm::vec3(xo, -row*yi,0),   glm::vec4(0.2f, 0.2f, 1.f, 1.f), scale, scale,glm::mat4(1.f));
				Marshal::FreeHGlobal(p);
			}

			
			static void DrawInWorld(String^ text,glm::vec3& pos, Colour^ colour, float scalex, float scaley, glm::mat4& transform,bool billboard)
			{
				
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				const char* str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderInWorld(str, pos.x,pos.y,pos.z, transform, colour->glm(), scalex, scaley,billboard);
				Marshal::FreeHGlobal(p);
			}
			
			void RenderNative(String^ text, float x, float y, float z, glm::vec4 colour, float scalex, float scaley, bool inWorld, bool billboard, glm::mat4 transform)
			{
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				const char* str = static_cast<const char*>(p.ToPointer());
				if (inWorld)
				{
					glm::mat4 proj = glx::Render::Default->MVP*glm::translate(transform,glm::vec3(x,y,z));
					FontManager::current->freeSans->RenderText(str, glm::vec3(),colour, scalex, scaley,proj);
					FontManager::current->freeSans->projection = FontManager::current->freeSans->defaultProj;
				}
				else
					FontManager::current->freeSans->RenderText(str,x, y, colour, scalex, scaley);
				Marshal::FreeHGlobal(p);
			}
		};
	}
}
