#pragma once
#include "SystemHeader.h" //<- prevent win.h/c#form conflicts
#include "GlaxionHeader.h"
#include "glaxRenderSettings.h"

namespace Antimatter {
	namespace Games {

		public ref class RenderSetting
		{
		public:
			RenderSetting()
			{
				name = "Render Setting";
				textureID = 0;
				lightingEnabled = true;
				screenSpace = false;
				wireFrame = false;
				solid = true;
				points = false;
				disableCulling = false;
				frontFaceCulling = false;
				backFaceCulling = true;
				drawAABB = false;
				drawAABBinfront = false;
				onTop = false;
				is2d = false;
			};

			String^ name;
			int textureID;
			bool lightingEnabled;
			bool screenSpace;
			bool wireFrame;
			bool solid;
			bool points;
			bool disableCulling;
			bool backFaceCulling;
			bool frontFaceCulling;
			bool drawAABB;
			bool drawAABBinfront;
			bool onTop;
			bool is2d;
			//static RenderSetting^ DefaultRenderSetting = gcnew RenderSetting();

			RenderSetting(glx::RenderSetting setting)
			{
				name = gcnew String(setting.name);
				textureID = setting.textureID;
				lightingEnabled = setting.lightingEnabled;
				screenSpace = setting.screenSpace;
				wireFrame = setting.wireFrame;
				solid = setting.solid;
				points = setting.points;
				disableCulling = setting.disableCulling;
				backFaceCulling = setting.backFaceCulling;
				frontFaceCulling = setting.frontfaceCulling;
				onTop = setting.onTop;
				is2d = setting.is2D;
			};

			glx::RenderSetting ToGLX()
			{
				glx::RenderSetting s;
				//	s.name = Glaxion::Tools::CoreTool::ToCstring(name);
				//Tool::Debug(gcnew String(s.name));
				s.textureID = textureID;
				s.lightingEnabled = lightingEnabled;
				s.screenSpace = screenSpace;
				s.wireFrame = wireFrame;
				s.solid = solid;
				s.points = points;
				s.backFaceCulling = backFaceCulling;
				s.disableCulling = disableCulling;
				s.frontfaceCulling = frontFaceCulling;
				s.onTop = onTop;
				s.is2D = is2d;
				return s;
			}
		};
	}
}
