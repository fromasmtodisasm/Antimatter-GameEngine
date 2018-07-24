#include "FontShader.h"
#include "glaxRenderer.h"

using namespace Antimatter::glx;

FT_Library FontShader::FreeTypeLibrary = NULL;
bool FontShader::FreeTypeInitialised = false;

Antimatter::glx::FontShader::FontShader()
{
	loadingError = false;
	shaderAvailable = false;
	gCount = 0;
}

void Antimatter::glx::FontShader::StartFontShader()
{
	vertexString =
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
						gl_Position = vec4(position.xy+coord.xy,position.z, 1);
					if(setting == 1)
						gl_Position = projection*vec4(coord.xy,0,1);
					if(setting == 2)
						gl_Position = projection*vec4(coord.xy+position.xy,position.z, 1);
					fragcoord = coord.zw;
				}
			)";
	fragmentString =
		R"(
				#version 420

				in vec2 fragcoord;
				uniform sampler2D tex;
				uniform vec4 colour;
				out vec4 final_colour;
				vec4 finalcolour;
				void main(void) 
				{
					finalcolour = vec4(1, 1, 1, texture2D(tex, fragcoord).r);
					final_colour = finalcolour*colour;
					//final_colour = colour;
				}
			)";

	geometryString = NULL;
	name = "Font Shader";
	Compile();
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffer_maximum, NULL, GL_DYNAMIC_READ);
	shaderAvailable = true;
	projection = glm::mat4(1.f);
	defaultProj = glm::ortho(0.f, (float)Core::windowWidth, 0.f, (float)Core::windowHeight, -1.f, 1.f);
	Init();
}

void Antimatter::glx::FontShader::LoadFont(const char * Path)
{
	fontPath = Path;
	FT_Error error = FT_New_Face(FreeTypeLibrary, Path,0,&face);
	if (error)
	{
		cout << "FontShader::LoadFont: Could not open Font: " << Path << endl;;
		return;
	}
	else
	{
		cout<<"FontShader::LoadFont:  Success: "<< Path<<endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);  
	//int h = 100;
	//FT_Set_Char_Size(face, h << 6, h << 6, 96, 96);
}

unsigned int Antimatter::glx::FontShader::GetGlyph(char Character)
{
	if (glyphs.find(Character) != glyphs.end())
	{
		return glyphs.at(Character);
	}
	else {
		unsigned int tid = GlyphShot::LoadGlyph(face, Character);
		if (tid > 0)
		{
			glyphs.insert(std::make_pair(Character, tid));
			gCount++;
			return tid;
		}
		else
			return 0; //error, return 0
	}
	return 0;
}

void Antimatter::glx::FontShader::Close()
{
	FT_Done_Face(face);
}

void Antimatter::glx::FontShader::RenderText(const char* text, float x, float y, const glm::vec4& col, float scalex, float scaley)
{
	RenderText(text, glm::vec3(x, y, 0), col, scalex, scaley,glm::mat4(1.f)); 
}

void Antimatter::glx::FontShader::RenderText(const char* text, const glm::vec3& pos, const glm::vec4& col, float scale_x, float scale_y,const glm::mat4& proj)
{
	Select();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	SetUniform3f(uposition, pos.x, pos.y, pos.z);
	SetUniform4f(ucolour, col.x, col.y, col.z, col.a);

	bool useProjection = false;
	if (useProjection)
	{
		SetUniform4x4(uprojection, defaultProj);
		SetUniform1i(usetting, 1);
	}
	else
	{
		scale_x /= 500.f;
		scale_y /= 500.f;
		SetUniform1i(usetting, 0);
	}
	
	
	Core::SelectBuffer(VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized
		4 * sizeof(GLfloat),                  // stride
		(void*)0            // array buffer offset
	);

	GLuint offsetx = 0;
	float offsety = 0.f;
	const char* p;
	//fix the scale of NDC font relative to the aspect ratio
	//https://stackoverflow.com/questions/48411444/resize-a-circle-in-opengl-keeping-the-aspect-ratio?rq=1
	//aspectRatio = w / h = 1.77778
	//1.0 / 1.77778 == 0.5625
	//X' = X * prjMat[0][0] = X * 0.5625/nRange
	//Y' = Y * prjMat[1][1] = Y * 1.0/nRange
	// nRange is assumed to be 1, therefore:
	//X = X*0.5625 (1/aspect ratio)
	//Y = Y*1.0/1.0 (cancels outs)
	float ratio = 1.f/((float)Core::windowWidth/(float)Core::windowHeight);
	scale_x *= ratio;

	if (FT_Load_Char(face, text[0], FT_LOAD_RENDER))
	{
		std::cout << "FontShader::RenderText::ERROR::FREETYTPE: Failed to load Glyph: " << text[0] << std::endl;
		return;
	}

	for (p = text; *p; p++)
	{
		int textureID = GetGlyph(p[0]);
		if (textureID < 1)
			continue;

		if (FT_Load_Char(face, p[0], FT_LOAD_RENDER))
		{
			std::cout << "FontShader::RenderInWorld::ERROR::FREETYTPE: Failed to load Glyph: " << p[0] << std::endl;
			continue;
		}

		FT_GlyphSlot ch = face->glyph;
		GLfloat xpos = (pos.x + offsetx)*scale_x;
		GLfloat ypos = (pos.y - (ch->bitmap.rows - ch->bitmap_top))*scale_y;
		GLfloat w = (ch->metrics.width / 64.f)*scale_x;
		GLfloat h = (ch->metrics.height / 64.f)*scale_y;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		offsetx += ch->advance.x >> (6 + ch->metrics.width);
		
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//TODO:  Need to fix projection states
void Antimatter::glx::FontShader::RenderInWorld(const char * text, float x,float y,float z, glm::mat4& transform, const glm::vec4& col,float scale_x, float scale_y,bool billboard)
{
	Select();
	float offsetx = 0.f;
	float offsety = 0.f;
	float s = 100.f;  //metric scale
	
	glm::mat4 view = Render::Default->viewMatrix;
	glm::mat4 projection = Render::Default->projectionMatrix;
	glm::mat4 transformation = glm::mat4(1.f);
	glm::mat4 modelView = view*glm::translate(transform, glm::vec3(x,y,z));
	
	if (billboard)
	{
		transformation[3][0] = modelView[3][0];
		transformation[3][1] = modelView[3][1];
		transformation[3][2] = modelView[3][2];
		transformation[3][3] = modelView[3][3];
		transformation = projection*transformation;
		scale_x /= s;
		scale_y /= s;
	}
	else
	{
		transformation = projection*view*transform;
		scale_x /= 2.f;
		scale_y /= 2.f;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	SetUniform3f(uposition, x, y, z);
	SetUniform4f(ucolour, col.x, col.y, col.z, col.a);

	if (billboard)
		SetUniform1i(usetting, 1);
	else
		SetUniform1i(usetting, 2);
	SetUniform4x4(uprojection, transformation);

	Core::SelectBuffer(VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		4,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		4 * sizeof(GLfloat),                  // stride
		(void*)0            // array buffer offset
	);

	if (FT_Load_Char(face, text[0], FT_LOAD_RENDER))
	{
		std::cout << "FontShader::RenderInWorld::ERROR::FREETYTPE: Failed to load Glyph: " << text[0] << std::endl;
		return;
	}

	const char* p;
	for (p = text; *p; p++)
	{
		GLuint textureID = GetGlyph(p[0]);
		if (textureID < 1)
			continue;
		//textures don't load properly without this for some reason.  
		//need to investigate
		if (FT_Load_Char(face, p[0], FT_LOAD_RENDER))
		{
			std::cout << "FontShader::RenderInWorld::ERROR::FREETYTPE: Failed to load Glyph: " << p[0] << std::endl;
			continue;
		}

		FT_GlyphSlot ch = face->glyph;
		GLfloat xpos = (x + offsetx)*scale_x;
		GLfloat ypos = (y - (ch->bitmap.rows - ch->bitmap_top))*scale_y;
		GLfloat w = (ch->metrics.width / 64.f)*scale_x;
		GLfloat h = (ch->metrics.height / 64.f)*scale_y;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		offsetx += ch->advance.x >> (6 + ch->metrics.width);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glDisableVertexAttribArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Antimatter::glx::FontShader::GetUniforms()
{
	uprojection = glGetUniformLocation(id, "projection");
	uposition = glGetUniformLocation(id, "position");
	usetting = glGetUniformLocation(id, "setting");
	ucolour = glGetUniformLocation(id, "colour");
}