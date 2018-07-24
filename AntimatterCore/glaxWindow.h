#pragma once
#include "SystemHeader.h"
#define WIDTH           1080
#define HEIGHT          720
#define WIDTH_DIV_2      (WIDTH*0.5f)
#define HEIGHT_DIV_2     (HEIGHT*0.5f)
#define ASPECT            1.3333f

namespace Antimatter{
	namespace glx{
		class Window
		{
		private:

		public:
			Window()
			{
				height = HEIGHT;
				width = WIDTH;
				ratio = ASPECT;
				fieldOfView = 90;
				title = "Window";
			}

			Window(const Window& w)
			{
				width = w.width;
				height = w.height;
				ratio = w.ratio;
				fieldOfView = w.fieldOfView;
				title = w.title;
				iconPath = w.iconPath;
			}

			float width;
			float height;
			float ratio;
			float fieldOfView;
			const char* title;
			const char* iconPath;

			static Window* defaultWindow;
		};
	}
}
