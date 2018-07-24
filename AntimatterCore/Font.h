#pragma once

#include "SystemHeader.h"
#include <windows.h>		// Header File For Windows
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

// 2nd Counter Used To Move Text & For Coloring
//HMODULE FH = LoadLibrary(_T("freetype271d.dll"));

#include <ft2build.h>
#include FT_FREETYPE_H

#include "glaxTexture.h"
#include "glaxCore.h"
#include "glaxShader.h"
#include "glaxRenderer.h"
#include "FontManager.h"
#include "FontNativer.h"

namespace Antimatter
{
	namespace Games 
	{
		//class represents a an entire font
		//all it's glyphs
		//freesans is our default font
		public ref class Font
		{
		public:

			ref struct Glyph
			{
			public:
				Glyph(char FontCharacter,int TextureID)
				{
					character = FontCharacter;
					textureID = TextureID;
				};
				char character;
				int textureID;
			};
			//TODO
			//implement proper destructor pattern
			!Font()
			{
				
			}

			~Font()
			{
				face = NULL;
				delete face;
			}

			Font(){};
			String^ name;
			String^ path;
			FT_Face* face;
			static Font^ FreeSans;
			bool FontsInitialized;
			Dictionary<char, Glyph^>^ glyphs = gcnew Dictionary<char, Glyph^>();
		};
	}
}

