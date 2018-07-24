#pragma once
#include "SystemHeader.h"
#include "SOIL\SOIL.h"
#include "SOIL\image_helper.h"

//https://www.opengl.org/discussion_boards/showthread.php/176158-Texture-Mapping-Loading-an-image-using-SOIL

//http://wiki.lwjgl.org/wiki/GLSL_Tutorial:_Texturing

//https://www.opengl.org/wiki/Geometry_Shader_Examples


namespace Antimatter{
	namespace glx{

		class Texture
		{
		private:

		public:

			Texture()
			{
				id = 0;
			}

			Texture(const char* filePath)
			{
				CopyFrom(LoadFromFile(filePath));
			};

			Texture(const Texture& t)
			{
				data = t.data;
				id = t.id;
				width = t.width;
				height = t.height;
				path = t.path;
			};

			void CopyFrom(Texture t)
			{
				data = t.data;
				id = t.id;
				width = t.width;
				height = t.height;
				path = t.path;
			}

			unsigned char* data;
			unsigned int id;
			const char * path;
			int width;
			int height;

			static Texture LoadFromFile(const char * filePath)
			{
				if (filePath == NULL)
				{
					cout << "\nnative texture: LoadFromFile: error: path is invalid" << endl;
					return NULL;
				}
				cout << filePath << endl;
				int width, height;
				unsigned char* imagedata =
					SOIL_load_image(filePath, &width, &height, 0, SOIL_LOAD_RGBA);

				if (imagedata == nullptr)
					cout << " \n glaxTexture: failed to load image.  Check the path\n" << endl;
				GLuint texture;
				
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, imagedata);
				
				SOIL_free_image_data(imagedata);

				Texture t;
				t.path = filePath;
				t.id = texture;
				t.width = width;
				t.height = height;
				t.data = imagedata;
				glBindTexture(GL_TEXTURE_2D, 0);
				return t;
			}

			static unsigned int Load(const char* filePath)
			{
				Texture t = LoadFromFile(filePath);
				return t.id;
			}

			void Bind()
			{
				if (id == 0)
					cout << "\n glxTexture: Warning, texture id is 0" << endl;
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, id);
			}
			
			~Texture()
			{
				glDeleteTextures(1, &id);
			};
		};
	}
}
