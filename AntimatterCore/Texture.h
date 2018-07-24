#pragma once
#include "GlaxionHeader.h"
#include "SystemHeader.h"
#include "glaxTexture.h"
#include "GImage.h"

using namespace System::Runtime::InteropServices;

namespace Antimatter {
	namespace Games {

		public ref class Texture
		{
		private:

			static void AddImage(GImage^ image)
			{
				Images->Add(image);
			}

			static GImage^ LoadGImage(String^ path)
			{
				return gcnew GImage(path);
			}

			static bool ContainsTexture(String^ path)
			{
				for each(GImage^ i in Images)
				{
					if (i->filePath == path)
					{
						return true;
					}
				}
				return false;
			}

			static GImage^ GetImage(String^ path)
			{
				for each(GImage^ i in Images)
				{
					if (i->filePath == path)
					{
						return i;
					}
				}

				GImage^ newi = LoadGImage(path);
				AddImage(newi);
				return newi;
			}

		public:
			Texture()
			{
			};

			unsigned int id;
			String^ filePath;
			static List<GImage^>^ Images = gcnew List<GImage^>();

			Texture(String^ filePath)
			{
				Load(filePath);
			}

			int Load(String^ path)
			{
				GImage^ g = GetImage(path);
				id = g->ID;
				filePath = path;
				return id;
			}

			static Texture^ GetTexture(String^ path)
			{
				return gcnew Texture(path);
			}
		};
	}
}

