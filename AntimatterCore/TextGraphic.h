#pragma once
#include "SystemHeader.h"
#include "Graphic.h"
#include "FontManager.h"

namespace Antimatter {
	namespace Games {

		public ref class ScreenText : public Graphic
		{
		public:

			String^ text;
			Colour^ colour;
			float scaleX;
			float scaleY;
			float x;
			float y;
			bool useProjection;

			ScreenText()
			{
				String^ text = "empty text";
				Colour^ colour = Colour::Red;
				float scaleX = 1.f;
				float scaleY = 1.f;
				float x = 0.f;
				float y = 0.f;
				bool useProjection= false;
			}

			ScreenText(String^ s,Colour^ col,float X, float Y,float scalex,float scaley) : Graphic()
			{
				text = s;
				colour = col;
				x = X;
				y = Y;
				scaleX = scalex;
				scaleY = scaley;
				useProjection = false;
			}

			void Draw() override
			{
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				const char* str = static_cast<const char*>(p.ToPointer());
//				FontManager::current->freeSans->useProjection = useProjection;
				FontManager::current->freeSans->RenderText(str, x, y, colour->glm(), scaleX, scaleY);
				Marshal::FreeHGlobal(p);
			}
		};

		public ref class WorldText : public Graphic
		{
		public:

			WorldText() : Graphic()
			{

			};

			String^ text;
			Colour^ colour;
			Colour^ colour2;
			float scaleX;
			float scaleY;
			float x;
			float y;
			float z;
			bool billboard;
			bool useProjection;

			WorldText(String^ s, Colour^ col, const glm::vec3& vec,float scalex, float scaley,bool isBillboard) : Graphic()
			{
				text = s;
				colour = col;
				x = vec.x;
				y = vec.y;
				z = vec.z;
				scaleX = scalex;
				scaleY = scaley;
				useProjection = true;
				billboard = isBillboard;
			}

			void Draw() override
			{
				const char* str;
				IntPtr p = Marshal::StringToHGlobalAnsi(text);
				str = static_cast<const char*>(p.ToPointer());
				FontManager::current->freeSans->RenderInWorld(str, x,y,z, glm::mat4(1.f), colour->glm(), scaleX, scaleY, billboard);
				Marshal::FreeHGlobal(p);
			}
		};
	}
}

