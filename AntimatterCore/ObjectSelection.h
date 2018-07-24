#pragma once
#include "SystemHeader.h"
#include"GlaxionHeader.h"
#include "Colour.h"

//generating proper colour IDs
//http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-an-opengl-hack/

namespace Antimatter {
	namespace Games
	{

		public ref class ObjectSelection
		{
		public:
			ObjectSelection() {};

			static List<Colour^>^ colourIDs = gcnew List<Colour^>();
			static float R = 0.f;
			static float G = 0.f;
			static float B = 0.f;
			static float A = 1.f;
			static float increment = 0.1f;
			/*
			static void ClearIDs()
			{
				colourIDs->Clear();
				R = 0.f;
				G = 0.f;
				B = 0.f;
				A = 1.f;
			}
			*/

			static bool IsID(Colour^ col1,Colour^ col2)
			{
				if (col1->Round() == col2->Round())
					return true;
				return false;
			}

			static bool ContainsID(Colour^ col)
			{
				for each(Colour^ c in colourIDs)
				{
					if (IsID(c, col))
						return true;
				}
				return false;
			}

			static int CreateColourID()
			{
				int i = colourIDs->Count;
				int r = (i & 0x000000FF) >> 0;
				int g = (i & 0x0000FF00) >> 8;
				int b = (i & 0x00FF0000) >> 16;
				//int a = (i & 0xFF000000) >> 24;
				Colour^ raw = gcnew Colour(r/255.f,g/255.f,b/255.f,1.f);
				colourIDs->Add(raw);
				return i;
			}
		};
	}
}

