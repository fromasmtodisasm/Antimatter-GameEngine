#pragma once

#include "glaxShader.h"
#include "FontNativer.h"
#include <map>

//https://www.freetype.org/freetype2/docs/tutorial/step1.html
//nehe tut
//http://nehe.gamedev.net/tutorial/freetype_fonts_in_opengl/24001/
//nehe bug fix
//https://www.gamedev.net/forums/topic/612246-bug-in-the-freetype-opengl-tutorial/

namespace Antimatter {
	namespace glx {

		public class FontShader : public Shader
		{
		public:
			FontShader();

			std::map<char, GLuint> glyphs;
			FT_Face face;
			static FT_Library FreeTypeLibrary;
			static bool FreeTypeInitialised;
			bool shaderAvailable;
			bool loadingError;

			static void Init()
			{
				if (FreeTypeInitialised)
					return;

				FT_Error error = FT_Init_FreeType(&FreeTypeLibrary);
				
				if (error == 0)
				{
					FreeTypeInitialised = true;
					cout << "\nFontShader::Init: FreeType library initialized" << endl;
				}else{
					cout<<"\nFontShader::Init: Could not init freetype library: error: "<<error<<endl;
				}
				return;
			}

			static void Shutdown()
			{
				FT_Done_FreeType(FreeTypeLibrary);
			}
			
			glm::mat4 projection;
			glm::mat4 defaultProj;
			const char* fontPath;
			int gCount;
			GLuint VBO;
			void LoadFont(const char* Path);
			void RenderText(const char* text,const glm::vec3&,const glm::vec4& col,float  scale_x,float scale_y, const glm::mat4& projection);
			void RenderText(const char* text, float x, float y, const glm::vec4& col, float scalex, float scaley);
			void RenderInWorld(const char* text, float x,float y,float z,glm::mat4& proj,const glm::vec4& col, float scale_x,float scale_y,bool billboard);
			unsigned int GetGlyph(char Character);
			void Close();
			GLuint uprojection;
			GLuint uposition;
			GLuint usetting;
			GLuint ucolour;
			virtual void GetUniforms();
			void StartFontShader();
			const char* vertexShaderLight = R"(#version 420

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
			uniform vec4 Colour;
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

				passcol = Colour;
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

			const char * fragmentShaderLight = R"(
			#version 420

			vec4 passcol;
			vec3 passnorm;
			vec3 passpos;
			vec3 passtolight;
			vec2 passuv;
			out vec4 outcolour;
			uniform vec3 lightposition;
			uniform vec4 lightintensity;
			uniform vec4 lightatten;
			uniform vec4 ambient;
			uniform vec3 eyeposition;
			layout(binding=0) uniform sampler2D tex;
			layout(binding=1) uniform sampler2D glowTex;
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
				vec4 finalcolour = vec4(1,1,1,1);

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
				glowTex = vec4(0);
			}

			//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
			)";

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
			uniform mat4 transform;
			uniform mat4 MVP;

			void main() {

			  for(int i = 0; i < 3; i++)
			  { 
				frag.normal = vertices[i].normal;
				frag.color = vertices[i].color;
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
		};
	}
}
