#include "glaxFont.h"
//#include <ft2build.h>
//#include FT_FREETYPE_H


namespace Glaxion {
	namespace glx
	{
		GLuint NativeFont::StartTextShader()
		{
			cout << "glax Font:: creating text shader" << endl;
			shaderID = glCreateProgram();
			glx::Shader  s;
			Shader::CreateFromString(shaderID, GL_VERTEX_SHADER, GetVertexString());
			//Shader::CreateFromString(shaderID, GL_GEOMETRY_SHADER, geometryText);
			Shader::CreateFromString(shaderID, GL_FRAGMENT_SHADER, GetFragmentString());

			//glCompileShader(shaderID);
			Shader::DebugShaders(shaderID);

			orthographic = glm::ortho(0.f, (float)Core::windowWidth, 0.f, (float)Core::windowHeight, -1.f, 100.f);
			perspective = &Render::DefaultRender->MVP;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 100000, NULL, GL_DYNAMIC_READ);

			return shaderID;
		}


		CharacterOld NativeFont::MakeTexture(char c)
		{
			CharacterOld character;
			
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "glaxFont::MakeTexture::ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				character.glyph = NULL;
				return character;
			}
			// Generate texture
			GLuint texture;
		//	glActiveTexture(GL_TEXTURE0);
			//glActiveTexture(GL_TEXTURE1);
			glGenTextures(1, &texture);
			//glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture);
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
			
			character.textureID = texture;
			character.glyph = face->glyph;
			character.character_ = c;
			//AddCharacter(character);
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, 0);
			// Now store character for later use
			return character;
		}

	
		const char* NativeFont::GetVertexString()
		{
			const char* vs =
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
					{
						gl_Position = vec4(coord.xy+position.xy,0,1);
					}
					if(setting == 1)
						gl_Position = projection*vec4(position.x+coord.x,position.y+coord.y,position.z, 1);
					fragcoord = coord.zw;
				}

			)";
			return vs;
		}
		const char* NativeFont::GetFragmentString()
		{
			const char* fs =
				R"(
				#version 420

				in vec2 fragcoord;
				uniform sampler2D tex;
				uniform vec4 color;
				out vec4 final_color;
				vec4 finalcolor;
				void main(void) 
				{
					finalcolor = vec4(1, 1, 1, texture2D(tex, fragcoord).r);
					final_color = finalcolor * color;
				}

			)";
			return fs;
		}

		void NativeFont::RenderText(const char* text, float x, float y, glm::vec4 col, float scalex, float scaley)
		{
			return;
			//glEnable(GL_TEXTURE_2D);
			//glActiveTexture(GL_TEXTURE0+1);
			//x += glx::Core::windowWidth / 2;
			//y += glx::Core::windowHeight / 2;
			//	x = 0;
			//	y = 0;
			glm::mat4 proj = glm::mat4(1.f);
			//proj = orthographic;
			//glViewport(0, 0, 1, 1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//glDisable(GL_CULL_FACE);
			glUseProgram(shaderID);
			glProgramUniform3f(shaderID, glGetUniformLocation(shaderID, "position"), x,y, 0);
			glProgramUniform4f(shaderID, glGetUniformLocation(shaderID, "color"), col.x, col.y, col.z, col.a);
			glProgramUniformMatrix4fv(shaderID, glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &proj[0][0]);
			glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, "setting"), 0);
			glEnableVertexAttribArray(0);
			//glProgramUniform3f(shaderID, glGetUniformLocation(shaderID, "color"), 1, 0, 1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//	glBindVertexArray(aVAO);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				4*sizeof(GLfloat),                  // stride
				(void*)0            // array buffer offset
			);



			// Iterate through all characters
			const char *p;
			GLuint offsetx = 0;
			float offsety = 0;
			//float s = (scale/100.f);
			float s = 100 * scalex;

			if (FT_Load_Char(face, 'G', FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			}
			FT_GlyphSlot T = face->glyph;
			unsigned int i = 0;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//scalex = 1 / 100;
			//scaley = 1 / 100;
			for (p = text; *p; p++)
			{
				//	cout << text[i] << "  ";
				//const char value = text[i];

				//Character character = MakeTexture(p[0]);
				CharacterOld character = SearchForCharacter(p[0]);
				i++;
				if (character.glyph == NULL)
					continue;
				FT_GlyphSlot ch = character.glyph;

				GLfloat xpos = (x + offsetx)*scalex;
				GLfloat ypos = (y - (ch->bitmap.rows - ch->bitmap_top))*scaley;
				GLfloat w = (ch->metrics.width / 64.f)*scalex;
				GLfloat h = (ch->metrics.height / 64.f)*scaley;


				GLfloat vertices[6][4] = {
					{ xpos,     ypos + h,   0.0, 0.0 },
					{ xpos,     ypos,       0.0, 1.0 },
					{ xpos + w, ypos,       1.0, 1.0 },

					{ xpos,     ypos + h,   0.0, 0.0 },
					{ xpos + w, ypos,       1.0, 1.0 },
					{ xpos + w, ypos + h,   1.0, 0.0 }
				};
				//	offsetx += (ch->advance.x >> 6 + ch->bitmap.width);
				offsetx += (ch->advance.x >> 6 + ch->metrics.width);
				//	offsetx += (ch->advance.x >> 6 + (w/scale));

				/*
				cout << "glx::Font: glyph: " << glyph << endl;
				cout << "glx::Font: glyph width: " << w << endl;
				cout << "glx::Font: glyph height: " << h << endl;
				cout << "glx::Font: texture id: " << character.textureID << endl;
				*/


				//int tID = character.textureID;
				//int tuid = glGetUniformLocation(shaderID, "tex");
				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, tID);
				//glProgramUniform1i(shaderID,tuid,0);
				glBindTexture(GL_TEXTURE_2D, character.textureID);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, 0);
				// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				//x += (Advance << 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
			}

			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisable(GL_BLEND);
			//glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

		}

		void NativeFont::TestDisplay()
		{
			GLsizei width = 680;
			GLsizei height = 420;
			float sx = 2.0 / width;
			float sy = 2.0 / height;

			RenderText("Glaxion Engine", 0, 0, glm::vec4(1, 0, 0.2f, 0.5f), 1,1);
		}

		void NativeFont::RenderInWorld(const char* text, glm::vec3 pos, glm::vec4 col, float scaleX, float scaleY)
		{
			RenderInWorld(text, pos, glm::mat4(1.f),0,0,col, scaleX, scaleY, false,true);
		}

		void NativeFont::RenderInWorldBillboard(const char* text, glm::vec3 pos, float xOffset, float yOffset, glm::vec4 col, float scaleX, float scaleY)
		{
			return;
			//glEnable(GL_TEXTURE_2D);
			//glActiveTexture(GL_TEXTURE0+1);
		//	ClearFontCharacters();
		//	timer++;
			//cout << timer<<endl;
			if (timer > 150)
			{
				//ClearFontCharacters();
				//timer = 0;
			}
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			glUseProgram(shaderID);
			glProgramUniform4f(shaderID, glGetUniformLocation(shaderID, "color"), col.x, col.y, col.z, col.a);
			float offsetx = 0;
			float offsety = 0;
			float x = 0;
			float y = 0;
			pos += glm::vec3(0, yOffset, 0);
			glm::mat4 mvp = Render::DefaultRender->MVP;
			glm::mat4 transformation = glm::mat4(1.f);
			
				//scaleX /= 40;
			//	scaleY /= 40;
				glm::vec4 s_s = mvp* glm::vec4(pos, 1);
				scaleX /= s_s.w;
				scaleY /= s_s.w;
				s_s /= s_s.w;
				pos = glm::vec3(s_s);
				
			glProgramUniformMatrix4fv(shaderID, glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &transformation[0][0]);
			glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, "setting"), 1);

			glProgramUniform3f(shaderID, glGetUniformLocation(shaderID, "position"), pos.x, pos.y, pos.z);
			glEnableVertexAttribArray(0);
			//glProgramUniform3f(shaderID, glGetUniformLocation(shaderID, "color"), 1, 0, 1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//	glBindVertexArray(aVAO);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);



			// Iterate through all characters
			const char *p;

			//float s = (scale/100.f);
			float s = 100;
			scaleX /= s;
			scaleY /= s;

			if (FT_Load_Char(face, 'G', FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			}
			FT_GlyphSlot T = face->glyph;
			unsigned int i = 0;
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			for (p = text; *p; p++)
			{
				//	cout << text[i] << "  ";
				//const char value = text[i];
				
				CharacterOld character = SearchForCharacter(p[0]);
				//if(character.glyph == NULL)
				//Character 	character = MakeTexture(p[0]);
				i++;
				//if (&character == NULL)
				//	continue;
				if (character.glyph == NULL)
					continue;
				//cout << "\n"<<character.character_ << endl;
				FT_GlyphSlot ch = character.glyph;

				GLfloat xpos = (x + offsetx)*scaleX;
				GLfloat ypos = (y - (ch->bitmap.rows - ch->bitmap_top))*scaleY;
				GLfloat w = (ch->metrics.width / 64.f)*scaleX;
				GLfloat h = (ch->metrics.height / 64.f)*scaleY;


				GLfloat vertices[6][4] = {
					{ xpos,     ypos + h,   0.0, 0.0 },
					{ xpos,     ypos,       0.0, 1.0 },
					{ xpos + w, ypos,       1.0, 1.0 },

					{ xpos,     ypos + h,   0.0, 0.0 },
					{ xpos + w, ypos,       1.0, 1.0 },
					{ xpos + w, ypos + h,   1.0, 0.0 }
				};
				//	offsetx += (ch->advance.x >> 6 + ch->bitmap.width);
				offsetx += (ch->advance.x >> 6 + ch->metrics.width);
				//	offsetx += (ch->advance.x >> 6 + (w/scale));

				/*
				cout << "glx::Font: glyph: " << glyph << endl;
				cout << "glx::Font: glyph width: " << w << endl;
				cout << "glx::Font: glyph height: " << h << endl;
				cout << "glx::Font: texture id: " << character.textureID << endl;
				*/


				int tID = character.textureID;
				//int tuid = glGetUniformLocation(shaderID, "tex");
				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, tID);
				//glProgramUniform1i(shaderID,tuid,0);
				glBindTexture(GL_TEXTURE_2D, tID);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, 0);
				// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				//x += (Advance << 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
			}

			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisable(GL_BLEND);
			//glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		void NativeFont::RenderInWorld(const char* text, glm::vec3 pos, glm::mat4 trans, float xOffset, float yOffset,glm::vec4 col, float scaleX, float scaleY,bool billboard,bool drawpoint)
		{
			return;
			//glEnable(GL_TEXTURE_2D);
			//glActiveTexture(GL_TEXTURE0+1);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			glUseProgram(shaderID);
			glProgramUniform4f(shaderID, glGetUniformLocation(shaderID, "color"), col.x, col.y, col.z, col.a);
			float offsetx = 0;
			float offsety = 0;
			float x = 0;
			float y = 0;
			glm::mat4 view = Render::DefaultRender->viewMatrix;
			glm::mat4 proj = Render::DefaultRender->projectionMatrix;
			glm::mat4 mvp = Render::DefaultRender->MVP;
			glm::mat4 transformation = glm::mat4(1.f);
		//	float offsetx;
			//float offsety;
			if (drawpoint)
			{
				scaleX /= 40;
				scaleY /= 40;
				glm::vec4 s_s = mvp* glm::vec4(pos, 1);
				scaleX /= s_s.w;
				scaleY /= s_s.w;
				s_s /= s_s.w;
				x = xOffset;
				y = yOffset;
				//cout << "offsetx:   " << offsetx<<endl;
				//pos *= glm::vec3(scaleX, scaleY, 0);// *vec2(0.2, 0.05);
				pos = glm::vec3(s_s);// +glm::vec3(xOffset, yOffset, 0);
				//pos = pos + glm::vec3(scaleX*0.2f,scaleY*0.05f,0);
				
			}
			else {
				if (billboard)
				{
					//cout << "\n\n"<<text << endl;
					//cout << "\n  x " << pos.x << "\n  y " << pos.y << "\n  z " << pos.z;
					glm::mat4 modelView = view*(glm::translate(trans, pos));
					transformation[3][0] = modelView[3][0];
					transformation[3][1] = modelView[3][1];
					transformation[3][2] = modelView[3][2];
					transformation[3][3] = modelView[3][3];
					transformation = Render::DefaultRender->projectionMatrix*transformation;
					/*
					glm::mat4 modelView = view*trans;
					//	modelView[0][0] = trans[0][0];
					modelView[0][1] = 0.0f;
					modelView[0][2] = 0.0f;

					// Second colunm.
					modelView[1][0] = 0.0f;
					//	modelView[1][1] = trans[1][1];
					modelView[1][2] = 0.0f;

					// Thrid colunm.
					modelView[2][0] = 0.0f;
					modelView[2][1] = 0.0f;
					//	modelView[2][2] = trans[2][2];
					//modelView = glm::scale(modelView, glm::vec3(0.5f, 0.5f, 0.5f));
					//if (billboard)
					//	modelView = scale*modelView;
					transformation = proj*modelView;
					*/
				}
				else
					transformation = mvp*trans;
			}
				/*
				glm::mat4 modelView = view*trans;
				transformation[3][0] = modelView[3][0];
				transformation[3][1] = modelView[3][1];
				transformation[3][2] = modelView[3][2];
				transformation[3][3] = modelView[3][3];
				transformation = Render::DefaultRender->projectionMatrix*transformation;
				*/
			glProgramUniformMatrix4fv(shaderID, glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, &transformation[0][0]);
			glProgramUniform1i(shaderID, glGetUniformLocation(shaderID, "setting"), 1);

			glProgramUniform3f(shaderID, glGetUniformLocation(shaderID, "position"), pos.x, pos.y, pos.z);
			glEnableVertexAttribArray(0);
			//glProgramUniform3f(shaderID, glGetUniformLocation(shaderID, "color"), 1, 0, 1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			//	glBindVertexArray(aVAO);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				4,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);



			// Iterate through all characters
			const char *p;
			
			//float s = (scale/100.f);
			float s = 100;
			if (!drawpoint)
			{
				scaleX /= s;
				scaleY /= s;
			}

			if (FT_Load_Char(face, 'G', FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			}
			FT_GlyphSlot T = face->glyph;
			unsigned int i = 0;
			
			for (p = text; *p; p++)
			{
				//	cout << text[i] << "  ";
				//const char value = text[i];
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				CharacterOld character = SearchForCharacter(p[0]);
				//Character character = MakeTexture(p[0]);
				i++;
				if (character.glyph == NULL)
					continue;
				FT_GlyphSlot ch = character.glyph;

				GLfloat xpos = (x + offsetx)*scaleX;
				GLfloat ypos = (y - (ch->bitmap.rows - ch->bitmap_top))*scaleY;
				GLfloat w = (ch->metrics.width / 64.f)*scaleX;
				GLfloat h = (ch->metrics.height / 64.f)*scaleY;


				GLfloat vertices[6][4] = {
					{ xpos,     ypos + h,   0.0, 0.0 },
					{ xpos,     ypos,       0.0, 1.0 },
					{ xpos + w, ypos,       1.0, 1.0 },

					{ xpos,     ypos + h,   0.0, 0.0 },
					{ xpos + w, ypos,       1.0, 1.0 },
					{ xpos + w, ypos + h,   1.0, 0.0 }
				};
				//	offsetx += (ch->advance.x >> 6 + ch->bitmap.width);
				offsetx += (ch->advance.x >> 6 + ch->metrics.width);
				//	offsetx += (ch->advance.x >> 6 + (w/scale));

				/*
				cout << "glx::Font: glyph: " << glyph << endl;
				cout << "glx::Font: glyph width: " << w << endl;
				cout << "glx::Font: glyph height: " << h << endl;
				cout << "glx::Font: texture id: " << character.textureID << endl;
				*/


				//int tID = character.textureID;
				//int tuid = glGetUniformLocation(shaderID, "tex");
				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, tID);
				//glProgramUniform1i(shaderID,tuid,0);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
				glDrawArrays(GL_TRIANGLES, 0, 6);

				//glActiveTexture(GL_TEXTURE0);
				//glBindTexture(GL_TEXTURE_2D, 0);
				// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				//x += (Advance << 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
			}

			glDisableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDisable(GL_BLEND);
			//glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}

		void NativeFont::GetFont()
		{
			FT_Error error = FT_New_Face(ft, "FreeSans.ttf", 0, &face);
			if (error)
			{
				fprintf(stderr, "\n   ::glax Font: Could not open font\n");
				return;
			}
			else
			{
				fprintf(stderr, "\n   ::glax Font: Loaded FreeSans\n");

			}
			FT_Set_Pixel_Sizes(face, 0, 48);
		}
	}
}