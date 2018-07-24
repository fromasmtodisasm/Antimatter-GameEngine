#pragma once
#include "SystemHeader.h" //guard against namespace conflicts, otherwise do not need system header here
#include "GlaxionHeader.h"
#include "glaxTexture.h"
#include "Core.h"

namespace Antimatter{
	namespace Games{

		public ref class GImage
		{
		protected:
			!GImage()
			{
				nativeTexture = NULL;
				delete nativeTexture;
			}

		public:

			GImage(String^ filePath)
			{
				Load(filePath);
			}

			~GImage()
			{
				this->!GImage();
			}

			glx::Texture* nativeTexture;
			unsigned int ID;
			String^ filePath;
			unsigned int globalID;
			//eventually we can store bytes in this class

			void Load(String^ path)
			{
				std::string unmanaged = msclr::interop::marshal_as<std::string>(path);
				const char* text = unmanaged.c_str();
				nativeTexture = &glx::Texture::LoadFromFile(text);
				ID = nativeTexture->id;
				filePath = path;
			}
		};
	}
}

