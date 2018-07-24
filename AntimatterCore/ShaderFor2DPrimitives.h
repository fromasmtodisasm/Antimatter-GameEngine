#pragma once
#include "glaxShader.h"
#include "tool.h"

namespace Antimatter {
	namespace glx {

		class ShaderForPrimitives2D : public Shader
		{
		public:
			ShaderForPrimitives2D()
			{
				vertexString = vertexShader;
				geometryString = triangleGeometryShader;
				fragmentString = fragmentShader;
				name = "Triangle Shader";
			}
			const char* vertexShader = R"(#version 420
			void main()
			{
				
			}
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
					vec3 E = normalize(vec3(0,0,1)-frag.pos); // we are in Eye Coordinates, so EyePos is (0,0,0)  
					vec3 R = normalize(-reflect(L,N));
					vec4 specular = vec4(1.0,1.0,1.0,1.0);
					float shininess = 100;
					vec4 Ispec = specular * pow(max(dot(R,E),0.0),0.3*shininess);
					Ispec = clamp(Ispec, 0.0, 1.0); 
				
					finalColour = (frag.color*Idiff) + Ispec;
				}
				outcolour = frag.color;
				glowcolour = frag.glow;
			}


			//2D lighting
			//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
			//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
			)";

			//Adding line thickness
			//https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader
			const char* lineGeometryShader = R"(
			#version 420

			layout(triangles) in;
			layout(triangle_strip, max_vertices=6) out;

			out fData
			{
				vec3 pos;
				vec4 color;
				vec4 glow;
			}frag;
			
			uniform mat4[3] vertexMatrix;
			uniform mat4 transform;
			uniform float thickness;

			void main() 
			{
				mat4 vertexA = vertexMatrix[0];
				mat4 vertexB = vertexMatrix[1];
				vec4 PointA = vertexA[0].xyzw;
				vec4 PointB = vertexB[0].xyzw;
				
				vec4 colourA = vertexA[1].xyzw;
				vec4 colourB = vertexB[1].xyzw;

				vec4 gloA = vertexA[2].xyzw;
				vec4 gloB = vertexB[2].xyzw;

				vec4 segment = PointB - PointA;
				vec4 n = normalize(vec4(-segment.y,segment.x,0.0,0.0));
				float scale = thickness;
				vec4 extent = (n*0.01*thickness);
				vec4 a = PointA - extent;
				vec4 b = PointA + extent;
				vec4 c = PointB - extent;
				vec4 d = PointB + extent;

/////////////////First triangle of the line. //////////////////////////////////////

				frag.color = colourA;  
				frag.glow = gloA;
				frag.pos = vec3(a);
				gl_Position = transform*a;
				EmitVertex();

				frag.color = colourA;  
				frag.glow = gloA;
				frag.pos = vec3(b);
				gl_Position = transform*b;
				EmitVertex();

				frag.color = colourB;  
				frag.glow = gloB;
				frag.pos = vec3(c);
				gl_Position = transform*c;
				EmitVertex();

/////////////////Second triangle of the line. //////////////////////////////////////

				frag.color = colourB;  
				frag.glow = gloB;
				frag.pos = vec3(c);
				gl_Position = transform*c;
				EmitVertex();

				frag.color = colourB;  
				frag.glow = gloB;
				frag.pos = vec3(d);
				gl_Position = transform*d;
				EmitVertex();

				frag.color = colourA;  
				frag.glow = gloA;
				frag.pos = vec3(b);
				gl_Position = transform*b;
				EmitVertex();
			}

			//for drawing lines in clip space
			//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
			)";

			const char* triangleGeometryShader = R"(
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
			uniform mat4[3] vertexMatrix;

			vec4 GlowThershold(vec4 vertColor)		
			{
				float brightPassThreshold = 0.5;
				vec3 luminanceVector = vec3(0.2125, 0.7154, 0.0721);
				vec4 c = vertColor;
				float luminance = dot(luminanceVector, c.xyz);
				luminance = max(0.0, luminance - brightPassThreshold);
				c.xyz *= sign(luminance);
				//c.a = 1.0;
				return c;
			}

			void main() 
			{
				for(int i=0;i<3;i++)
				{
					mat4 mat = vertexMatrix[i];
					vec4 pos = mat[0].xyzw;
					vec4 col = mat[1].xyzw;
					vec4 glo = mat[2].xyzw;
					frag.pos = pos.xyz;
					frag.glow = glo;
					frag.color = col;
					gl_Position = transform*pos;
					EmitVertex();
				}
				EndPrimitive();
			}

			//for drawing lines in clip space
			//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
			)";

			unsigned int TransformID;
			unsigned int LightPositionID;
			unsigned int enableLightingID;
			unsigned int thicknessID;
			unsigned int vertexMatrix1ID;
			unsigned int vertexMatrix2ID;
			unsigned int vertexMatrix3ID;

			//managed to native transition
			//watch out for thunking
			virtual void GetUniforms() override
			{
				//	glUseProgram(id); //delete this line
				TransformID = glGetUniformLocation(id, "transform");
				LightPositionID = glGetUniformLocation(id, "lightposition");
				enableLightingID = glGetUniformLocation(id, "enableLighting");
				thicknessID = glGetUniformLocation(id, "thickness");
				vertexMatrix1ID = glGetUniformLocation(id, "vertexMatrix[0]");
				vertexMatrix2ID = glGetUniformLocation(id, "vertexMatrix[1]");
				vertexMatrix3ID = glGetUniformLocation(id, "vertexMatrix[2]");
			}

			virtual void Select() override
			{
				Shader::Select();
				glDisable(GL_CULL_FACE);
			}
		};
	}
}