#pragma once

#include "SystemHeader.h"
#include <windows.h>		
#include <math.h>			
#include <stdio.h>			
#include <stdarg.h>			// Header File For Variable Argument Routines
//#include <gl\gl.h>			
//#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glaux.h>		// Header File For The Glaux Library

// 2nd Counter Used To Move Text & For Coloring
//HMODULE FH = LoadLibrary(_T("freetype271d.dll"));
#include <ft2build.h>
#include FT_FREETYPE_H

#include "glaxTexture.h"
#include "glaxCore.h"

namespace Antimatter {
	namespace glx {

		public class GlyphShot
		{
		public:
			GlyphShot()
			{

			}

			int texture;
			char character;

			bool LoadFont(FT_Face& face)
			{
				if (FT_Load_Char(face, character, FT_LOAD_RENDER))
				{
					std::cout << "glaxFont::LoadFont::ERROR::FREETYTPE: Failed to load Glyph: " << character << std::endl;
					return false;
				}
				return true;
			}

			static unsigned int LoadGlyph(FT_Face& face,char Character)
			{
				if (FT_Load_Char(face, Character, FT_LOAD_RENDER))
				{
					std::cout << "glaxFont::MakeTexture::ERROR::FREETYTPE: Failed to load Glyph: " << Character<<std::endl;
					return 0;
				}

				// Generate texture
				GLuint textureid;
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glGenTextures(1, &textureid);
				glBindTexture(GL_TEXTURE_2D, textureid);

				// Set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				//glGenerateMipmap(GL_TEXTURE_2D);

				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);

				return textureid;
			}
		};
	}
}
