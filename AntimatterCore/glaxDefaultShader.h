#pragma once
#include "glaxShader.h"

namespace Antimatter {
	namespace glx {

		class DefaultShader : public Shader
		{
		public:
			DefaultShader()
			{
				vertexString = vertexShader;
				geometryString = geometryShader;
				fragmentString = fragmentShader;
			}
			const char* vertexShader = R"(#version 420

			in vec4 position;
			in vec3 normal;
			in vec2 uvin;
			out mat4 mvp;

			out vData
			{
				vec3 normal;
			vec4 color;
			vec2 uv;
			mat4 transform;
			}vertex;

			vec4 passcol;
			vec3 passnorm;
			vec3 passpos;
			vec3 passtolight;
			vec2 passuv;
			uniform mat4 viewmatrix;
			uniform mat4 projectionmatrix;
			uniform mat4 canvasmatrix;
			uniform vec4 colour;
			uniform mat4 transform;

			uniform mat4 positionmatrix;
			uniform mat4 scaledmatrix;

			uniform vec4 lightatten;
			uniform vec3 lightposition;
			uniform int settings[4];
			uniform vec2 screencoords;
			uniform vec2 refpos;

			void main()
			{

				if (settings[1] == 0)
				{
					gl_Position = transform*position;
				}

				passcol = colour;
				passnorm = vec3(scaledmatrix*vec4(normal, 0));
				passpos = vec3(positionmatrix*position);
				passtolight = (lightposition - vec3(position)) / lightatten.w;
				passuv = uvin;

				vertex.normal = passnorm;
				vertex.color = passcol;
				vertex.uv = passuv;
				vertex.transform = transform;
			}
			)";

			const char* geometryShader = R"(
			#version 420
			layout(triangles) in;
			layout(triangle_strip, max_vertices = 3) out;

			in vData
			{
				vec3 normal;
				vec4 color;
				vec2 uv;
				mat4 transform;
			}vertices[];

			out fData
			{
				vec3 pos;
				vec3 normal;
				vec4 color;
				vec2 uv;
			}frag;

			void main() 
			{
				for(int i = 0; i < 3; i++)
				{ 
					frag.normal = vertices[i].normal;
					frag.color = vertices[i].color;
					frag.uv = vertices[i].uv;
					frag.pos = vec3(gl_in[i].gl_Position);
					gl_Position = gl_in[i].gl_Position;

					EmitVertex();
				}
				EndPrimitive();
			}

			//transform feedback
			//http://stackoverflow.com/questions/14086926/get-results-of-gpu-calculations-back-to-the-cpu-program-in-opengl

			// glsl verison4 specs
			//https://www.opengl.org/registry/doc/GLSLangSpec.4.00.9.clean.pdf
			)";

			const char * fragmentShader = R"(
			#version 420

			vec4 passcol;
			vec3 passnorm;
			vec3 passpos;
			vec3 passtolight;
			vec2 passuv;
			layout(location=0) out vec4 outcolour;
			uniform vec3 lightposition;
			uniform vec4 lightintensity;
			uniform vec4 lightatten;
			uniform vec4 ambient;
			uniform vec3 eyeposition;
			uniform sampler2D tex;
			uniform int settings[4];

			in fData
			{
				vec3 pos;
				vec3 normal;
				vec4 color;
				vec2 uv;
			}frag;

			void main()
			{
				passcol = frag.color;
				passnorm = frag.normal;
				passpos = frag.pos;
				passuv = frag.uv;
				vec4 finalcolour = vec4(0.0,0.0,0.0,0.0);

				if(settings[3] == 1)//setting 3 use lighting
				{ 
					vec3 lightdir = normalize(lightposition - passpos);
					float brightness = max(dot(normalize(passnorm),lightdir),0.0f);
					vec4 diffuse = vec4(brightness,brightness,brightness,1)*lightintensity;
					diffuse = clamp(diffuse,0,1);

					vec3  reflectedlight = normalize(reflect(-lightdir,passnorm));
					vec3 eyevector =  normalize(eyeposition - passpos);
					float s = max(dot(reflectedlight,eyevector),0.0f);
					s = pow(s,50);
					vec4 specular = clamp(vec4(s,s,s,1),0,1);

					float dist = length(passtolight);
					float attenuation = lightatten.x +(lightatten.y*dist) + (lightatten.z * dist * dist);

					finalcolour = ambient + (((diffuse/attenuation) *passcol) + (specular/attenuation));
					finalcolour = ambient + diffuse*passcol + specular;
					float alpha = passcol.a;
					finalcolour = vec4(finalcolour.xyz,alpha);
				}else{
					finalcolour = passcol;
				}

				if(settings[0] != 0)  //textured
				{
					if(settings[2] == 1)
					{
						finalcolour = texture(tex,passuv)*passcol;
					}else{
						finalcolour = texture(tex,passuv)*finalcolour; //should add specular to textures but seems to be wrong
					}
					if (finalcolour.a < 0.01)
					{
						discard;
					}
				}
				//outcolour = frag.color;
				outcolour = finalcolour;
			}

			//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
			)";

			//Adding line thickness
			//https://forum.libcinder.org/topic/smooth-thick-lines-using-geometry-shader
			const char* lineGeometryShader = R"(
			#version 420

			layout(triangles) in;
			layout(line_strip, max_vertices = 3) out;

			in vData
			{
				vec3 normal;
				vec4 color;
				vec2 uv;
				mat4 transform;
			}vertices[];

			out fData
			{
				vec3 pos;
				vec3 normal;
				vec4 color;
				vec2 uv;
			}frag;

			uniform vec4 PointA;
			uniform vec4 PointB;
			uniform vec4 colour1;
			uniform vec4 colour2;
			uniform mat4 transform;
			uniform mat4 MVP;

			void main() 
			{

			  for(int i = 0; i < 2; i++)
			  { 
				frag.normal = vertices[i].normal;
				if(i==0)
					frag.color = colour2;  //flip final colours because line is draw from B to A
				else if(i==1)
					frag.color = colour1;
				
				frag.uv = vertices[i].uv;
				frag.pos = vec3(gl_in[i].gl_Position);
				gl_Position = vertices[i].transform*PointA;

				EmitVertex();
				gl_Position = vertices[i].transform*PointB;

				EmitVertex();
			  }
			  EndPrimitive();
			}

			//for drawing lines in clip space
			//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
			)";

			unsigned int mvpID;
			unsigned int TransformID;
			unsigned int ColourID;
			unsigned int AmbientLightID;
			unsigned int LightPositionID;
			unsigned int PositionMatrixID;
			unsigned int ScaledMatrixID;
			unsigned int LightIntensityID;
			unsigned int EyePositionID;
			unsigned int TextureID;
			unsigned int LightAttenID;
			unsigned int settingsID;
			unsigned int defaultTexture;
			unsigned int projectionID;
			unsigned int viewID;
			unsigned int screenCoordsID;
			unsigned int canvasMatrixID;
			unsigned int logicWorldPosID;
			unsigned int LinePointA;
			unsigned int LinePointB;
			unsigned int LineColour1;
			unsigned int LineColour2;

			virtual void GetUniforms() override
			{
			//	glUseProgram(id); //delet this line
				mvpID = glGetUniformLocation(id, "MVP");
				viewID = glGetUniformLocation(id, "viewmatrix");
				projectionID = glGetUniformLocation(id, "projectionmatrix");
				ColourID = glGetUniformLocation(id, "colour");
				TransformID = glGetUniformLocation(id, "transform");
				AmbientLightID = glGetUniformLocation(id, "ambient");
				LightPositionID = glGetUniformLocation(id, "lightposition");
				LightIntensityID = glGetUniformLocation(id, "lightintensity");
				PositionMatrixID = glGetUniformLocation(id, "positionmatrix");
				ScaledMatrixID = glGetUniformLocation(id, "scaledmatrix");
				EyePositionID = glGetUniformLocation(id, "eyeposition");
				LightAttenID = glGetUniformLocation(id, "lightatten");
				settingsID = glGetUniformLocation(id, "settings");
				screenCoordsID = glGetUniformLocation(id, "screencoords");
				canvasMatrixID = glGetUniformLocation(id, "canvasmatrix");
				logicWorldPosID = glGetUniformLocation(id, "logicWorldPos");
				LinePointA = glGetUniformLocation(id, "PointA");
				LinePointB = glGetUniformLocation(id, "PointB");
				LineColour1 = glGetUniformLocation(id, "colour1");
				LineColour2 = glGetUniformLocation(id, "colour2");
				TextureID = glGetUniformLocation(id, "tex");
			}
		};
	}
}