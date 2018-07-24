#include "glaxRenderer.h"

namespace Antimatter {
	namespace glx {

		void Render::InitializeShader()
		{
			defaultShader = glx::DefaultShader();
			defaultShader.name = "Mesh";
			defaultShader.Compile();
			defaultShader.Select();
			SetLightIntensity(LightIntensity.x, LightIntensity.y, LightIntensity.z);
			lineShader = glx::DefaultShader();
			lineShader.geometryString = lineShader.lineGeometryShader;
			lineShader.name = "Line";
			lineShader.Compile();
			
		}
	}
}