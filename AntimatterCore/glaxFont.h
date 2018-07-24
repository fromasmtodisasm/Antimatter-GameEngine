/*
*		This Code Was Created By Jeff Molofee 2000
*		Modified by Shawn T. to handle (%3.2f, num) parameters.
*		A HUGE Thanks To Fredric Echols For Cleaning Up
*		And Optimizing The Base Code, Making It More Flexible!
*		If You've Found This Code Useful, Please Let Me Know.
*		Visit My Site At nehe.gamedev.net
*/
#pragma once

#include "SystemHeader.h"
#include <windows.h>		// Header File For Windows
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
//#include <gl\gl.h>			// Header File For The OpenGL32 Library
//#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glaux.h>		// Header File For The Glaux Library

// 2nd Counter Used To Move Text & For Coloring
//HMODULE FH = LoadLibrary(_T("freetype271d.dll"));

#include <ft2build.h>
#include FT_FREETYPE_H

//#include "glaxRenderer.h"
#include "glaxTexture.h"
#include "glaxCore.h"
#include "glaxList.h"
#include "glaxShader.h"
#include "glaxRenderer.h"

//https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01
//https://learnopengl.com/#!Getting-started/Textures
//http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=12
//https://learnopengl.com/#!In-Practice/Text-Rendering

namespace Antimatter {
	namespace glx {

		struct BatchText
		{
		public:
			BatchText()
			{
				x = 0;
				y = 0;
				scalex = 1;
				scaley = 1;
				colour = glm::vec4(1, 0, 0, 1);
				text = "";
			}

			BatchText(const char* t,float xpos,float ypos,glm::vec4 col,float scale_x,float scale_y)
			{
				x = xpos;
				y = ypos;
				this->scalex = scale_x;
				this->scaley = scale_y;
				colour = col;
				text = t;
			}

			BatchText(const BatchText& xl)
			{
				x = xl.x;
				y = xl.y;
				scalex = xl.scalex;
				scaley = xl.scaley;
				colour = xl.colour;
				text = xl.text;
			}

			const char* text;
			float x;
			float y;
			float scalex;
			float scaley;
			glm::vec4 colour;
		};

		struct CharacterOld
		{
		public:
			CharacterOld()
			{

			}

			CharacterOld(char glyph)
			{
				character_ = glyph;
			}

			FT_GlyphSlot glyph = NULL;
			GLuint     textureID;  // ID handle of the glyph texture
			glm::ivec2 Size;       // Size of glyph
			glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
			GLuint     Advance;    // Offset to advance to next glyph
			char character_;

		};

		class NativeFont
		{
		public:
			
			const char* vertexString =
				R"(
				#version 420

				in vec4 coord;
				uniform mat4 projection;
				uniform vec3 position;
				uniform int setting;
				out vec2 fragcoord;
				void main(void)
				{
					if(setting == 0)
					gl_Position = projection*vec4(0,0,1, 1);
					if(setting == 1)
					gl_Position = projection*vec4(position.x+coord.x,position.y+coord.y,position.z, 1);
					fragcoord = coord.zw;
				}

			)";
			const char* fragmentString =
				R"(
				#version 420

				in vec2 fragcoord;
				uniform sampler2D tex;
				uniform vec4 color;
				out vec4 final_color;
				vec4 finalcolor;
				void main(void) 
				{
				//	vec4 finalcolor = texture(tex,fragcoord) * color;
					
					finalcolor = vec4(1, 1, 1, texture2D(tex, fragcoord).r);
					final_color = finalcolor * color;
					//vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, fragcoord));
					//finalcolor = vec4(color, 1.0) * sampled;
				}

			)";

			NativeFont()
			{
				//textureUnitID = 2;
			};

			FT_Library ft;
			FT_Face face;
			int textureUniformID;
			int attribute_coord;
			int colourID;
			GLuint defaultTexture;
			std::vector<BatchText> batchText;
			std::vector<CharacterOld> fontCharacters;
			int timer;

			const char* GetVertexString();
			const char* GetFragmentString();

			void Start()
			{
				
				FT_Error error = FT_Init_FreeType(&ft);
				if (error)
				{
					fprintf(stderr, "\n   ::glax Font: Could not init freetype library\n");
				}
				else {
					fprintf(stderr, "\n   ::glax Font: init freetype library\n");
				}
				Advance = 0;
				GetFont();
				StartTextShader();
				timer = 0;
			//	defaultTexture = glx::Texture::Load("defaultTexture.jpg");
				//cout << "\n::\n::\nglx::Font Loaded defaultTexure: " << defaultTexture<<endl;
				/*
				BatchText bt;
				bt.text = "Batch text";
				batchText.push_back(bt);
				cout << "batch Text Size: "<<batchText.size() << endl;
				cout<<"new line" << endl;
				*/
				//batchText.Add(bt);
			}

			void DrawBatchList()
			{
				//BatchText bt;
				//bt.text = "drawingtext";
				//RenderBatchLine(bt);
				//cout << "glx::Font: draw batch list" << endl;
				for (unsigned int i = 0; i < batchText.size(); i++)
				{
					RenderBatchLine(batchText[i]);
				}
				batchText.clear();
			}

			void ClearFontCharacters()
			{
				fontCharacters.clear();
			}

			void AddCharacter(CharacterOld c)
			{
				fontCharacters.push_back(c);
			}

			CharacterOld SearchForCharacter(char ch)
			{
				CharacterOld c = MakeTexture(ch);
				return c;
				for (unsigned int i = 0; i < fontCharacters.size(); i++)
				{
					if (fontCharacters[i].character_ == ch)
					{
						//cout << "\nReturning early with character: " << ch << endl;
						CharacterOld copy;
						copy.character_ = ch;
						copy.glyph = fontCharacters[i].glyph;
						copy.textureID = fontCharacters[i].textureID;
						copy.Size = fontCharacters[i].Size;
						copy.Advance = fontCharacters[i].Advance;
						copy.Bearing = fontCharacters[i].Bearing;
						return copy;
					}

				}
			//	Character c = MakeTexture(ch);
			//	AddCharacter(c);
			//	Character c= MakeTexture(ch);
				if (c.glyph == NULL)
				{
					//cout << "Why?" << endl;
				}
				else
				{
					//AddCharacter(c);
					//AddCharacter(c);
				}
				return c;
			}

			void BatchDrawText(const char* text, float x, float y, glm::vec4 col, float scalex,float scaley)
			{
				BatchText bt(text, x, y, col, scalex,scaley);
				batchText.push_back(bt);
			}

			void RenderBatchLine(BatchText batch)
			{
				RenderText(batch.text, batch.x, batch.y, batch.colour, batch.scalex,batch.scaley);
			}

			GLuint textureVBO;
			int shaderID;
			int fragmentShaderID;
			int vertexShaderID;
			unsigned int textureID;
			unsigned int vbo;
			unsigned int VAO;
			GLuint VBO;
			int Advance;
			glm::mat4 orthographic;
			glm::mat4* perspective;
			float defaultSize = 5;

			GLuint StartTextShader();

			CharacterOld NativeFont::MakeTexture(char c);

			void TestDisplay();
			void RenderInWorldBillboard(const char* text, glm::vec3 pos, float xOffset, float yOffset, glm::vec4 col, float scaleX, float scaleY);
			void RenderText(const char* text, float x, float y, glm::vec4 col, float scale_X, float scale_Y);
			void NativeFont::RenderInWorld(const char* text, glm::vec3 pos, glm::vec4 col, float scaleX, float scaleY);
			void NativeFont::RenderInWorld(const char* text, glm::vec3 pos, glm::mat4 trans, float xOoffset, float yOffset, glm::vec4 col, float scaleX, float scaleY, bool billboard, bool drawpoint);
			void GetFont();
			
		};
	}
}

