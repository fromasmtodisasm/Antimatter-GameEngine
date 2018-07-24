#pragma once
#include "glaxShader.h"
#include "tool.h"

namespace Antimatter {
	namespace glx {

		class ShaderForDiamonds2D : public Shader
		{
		public:
			ShaderForDiamonds2D()
			{
				vertexString = vertexShader;
				geometryString = diamondGeometryShader;
				fragmentString = fragmentShader;
				name = "Dimamond Shader";
			}
			const char* vertexShader = R"(#version 420
			void main()
			{
				
			}
			)";

			const char* diamondGeometryShader = R"(
			#version 420

			layout(triangles) in;
			layout(triangle_strip, max_vertices=6) out;

			out fData
			{
				vec3 pos;
				vec4 color;
				vec4 glow;
			}frag;

			uniform mat4 transform;
			uniform mat4 vertexMatrix;
			uniform float scale;

			void main() 
			{
				vec4 pos = vertexMatrix[0];
				
				vec4 colour = vertexMatrix[1];

				vec4 glo = vertexMatrix[2];
				float s = scale;
				vec4 bottom = transform*vec4(vec3(0.0,-1.0,0.0)*s+pos.xyz,1.0);
				vec4 right = transform*vec4(vec3(1.0,0.0,0.0)*s+pos.xyz,1.0);
				vec4 left = transform*vec4(vec3(-1.0,0.0,0.0)*s+pos.xyz,1.0);
				vec4 top = transform*vec4(vec3(0.0,1.0,0.0)*s+pos.xyz,1.0);

				frag.pos = bottom.xyz;
				frag.glow = glo;
				frag.color = colour;
				gl_Position = bottom;
				EmitVertex();

				frag.pos = right.xyz;
				frag.glow = glo;
				frag.color = colour;
				gl_Position = right;
				EmitVertex();

				frag.pos = left.xyz;
				frag.glow = glo;
				frag.color = colour;
				gl_Position = left;
				EmitVertex();

				EndPrimitive();

				frag.pos = top.xyz;
				frag.glow = glo;
				frag.color = colour;
				gl_Position = top;
				EmitVertex();

				frag.pos = left.xyz;
				frag.glow = glo;
				frag.color = colour;
				gl_Position = left;
				EmitVertex();

				frag.pos = right.xyz;
				frag.glow = glo;
				frag.color = colour;
				gl_Position = right;
				EmitVertex();
			
				EndPrimitive();
			}

			//for drawing lines in clip space
			//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
			)";

			//https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
			//https://learnopengl.com/Lighting/Basic-Lighting
			const char * fragmentShader = R"(
			#version 420

			layout(location = 0) out vec4 outcolour;
			layout(location = 1) out vec4 glowcolour;
			uniform vec3 lightposition;
			uniform vec4 lightintensity;
			uniform mat4 transform;
			uniform int enableLighting;

			in fData
			{
				vec3 pos;
				vec4 color;
				vec4 glow;
			}frag;

			void main()
			{
				vec4 finalColour = frag.color;

				if(enableLighting == 1)
				{
					vec3 N = vec3(0,0,1);
					vec3 L = normalize(lightposition - frag.pos);
					float Idiff = max(dot(N,L),0.0);
					Idiff = clamp(Idiff*Idiff*Idiff,0.0,1.0);

					// calculate Specular Term:
					vec3 E = normalize(-frag.pos); // we are in Eye Coordinates, so EyePos is (0,0,0)  
					vec3 R = normalize(-reflect(L,N));
					vec4 specular = vec4(1.0,1.0,1.0,1.0);
					float shininess = 100;
					vec4 Ispec = specular * pow(max(dot(R,E),0.0),0.3*shininess);
					Ispec = clamp(Ispec, 0.0, 1.0); 
				
					finalColour = (frag.color*Idiff) + Ispec;
				}
				outcolour = finalColour;
				glowcolour = frag.glow;
			}


			//2D lighting
			//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
			//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
			)";

			unsigned int TransformID;
			unsigned int LightPositionID;
			unsigned int enableLightingID;
			unsigned int scaleID;
			unsigned int vertexMatrixID;

			//managed to native transition
			//watch out for thunking
			virtual void GetUniforms() override
			{
				//	glUseProgram(id); //delete this line
				TransformID = glGetUniformLocation(id, "transform");
				LightPositionID = glGetUniformLocation(id, "lightposition");
				enableLightingID = glGetUniformLocation(id, "enableLighting");
				scaleID = glGetUniformLocation(id, "scale");
				vertexMatrixID = glGetUniformLocation(id, "vertexMatrix");
			}

			virtual void Select() override
			{
				Shader::Select();
//hack so we don't have to check the winding of the triangles drawn in the geo shader
				glDisable(GL_CULL_FACE);
			}
		};
	}
}