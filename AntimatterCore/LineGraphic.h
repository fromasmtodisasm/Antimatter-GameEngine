#pragma once
#include "SystemHeader.h" //<- prevent win.h conflicts
#include "GlaxionHeader.h"
#include "Colour.h"
#include "Graphic.h"
#include "glaxRenderer.h"

namespace Antimatter {
	namespace Games {

		public ref class LineGraphic : public Graphic
		{
		private:

		public:
			LineGraphic() : Graphic()
			{
				ax = 0.f;
				ay = 0.f;
				az = 0.f;

				bx = 0.f;
				by = 5.f;
				bz = 0.f;
				colour = gcnew Colour(1.f, 0.f, 0.f);
				colour2 = colour;
				width = 1.f;
				index = 0;
				drawOnTop = false;
				orthoGraphic = false;
			};

			LineGraphic(const glm::vec3& v1, const glm::vec3& v2, Colour^ col, bool onTop)
			{
				ax = v1.x;
				ay = v1.y;
				az = v1.z;

				bx = v2.x;
				by = v2.y;
				bz = v2.z;

				colour = col;
				colour2 = colour;
				index = 0;
				width = 1.f;
				drawOnTop = onTop;
				orthoGraphic = false;
			}

			LineGraphic(const glm::vec3& v1, const glm::vec3& v2, Colour^ col)
			{
				ax = v1.x;
				ay = v1.y;
				az = v1.z;

				bx = v2.x;
				by = v2.y;
				bz = v2.z;

				colour = col;
				colour2 = colour;
				index = 0;
				width = 1.f;
				drawOnTop = true;
				orthoGraphic = false;
			}

			LineGraphic(const glm::vec3& v1, const glm::vec3& v2, Colour^ col, Colour^ col2, bool onTop)
			{
				ax = v1.x;
				ay = v1.y;
				az = v1.z;

				bx = v2.x;
				by = v2.y;
				bz = v2.z;

				colour = col;
				colour2 = col2;
				index = 0;
				width = 1.f;
				drawOnTop = onTop;
				orthoGraphic = false;
			}

			LineGraphic(const LineGraphic% v)
			{
				ax = v.ax;
				ay = v.ay;
				az = v.az;

				bx = v.bx;
				by = v.by;
				bz = v.bz;
				
				colour = v.colour;
				colour2 = v.colour2;
				width = v.width;
				drawOnTop = v.drawOnTop;
				orthoGraphic = v.orthoGraphic;
				index = v.index;
			};

			void Draw() override
			{
				glm::vec3 a;
				a.x = ax;
				a.y = ay;
				a.z = az;

				glm::vec3 b;
				b.x = bx;
				b.y = by;
				b.z = bz;
				glx::Render::Default->DrawLine(a, b, colour->glm(), colour2->glm(), 1.f, drawOnTop, orthoGraphic);
			}
			
			float ax;
			float ay;
			float az;

			float bx;
			float by;
			float bz;

			Colour^ colour;
			Colour^ colour2;
			float width;
			bool drawOnTop;
			bool orthoGraphic;
			unsigned int index;
		};
	}
}

