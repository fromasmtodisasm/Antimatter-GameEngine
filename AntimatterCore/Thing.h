#pragma once
#include "GlaxionHeader.h"
#include "SystemHeader.h"
#include "glaxRenderSettings.h"
#include "Transform.h"
#include "Colour.h"
#include "glaxTexture.h"
#include "Texture.h"
#include "ObjectSelection.h"
#include "RenderSettings.h"

namespace Antimatter {
	namespace Games {

		public enum class Mode { Solid, Wireframe, Points };

		public ref class Thing : Transform
		{
		private:
			void Construction()
			{
				name           = "Thing";
				visible        = true;
				isTextured     = false;
				texture        = gcnew Texture();
				ScreenSpace    = false;
				GenerateID();
				renderSettings = gcnew RenderSetting();
				selectable     = true;
				SetColour(gcnew Colour(1, 0, 0, 1));
				mode = Mode::Solid;
			}

		public:
			Thing() : Transform()
			{
				Construction();
			}

			String^ fullPath;
			Texture^ texture;
			Colour^ colour;
			Colour^ defaultColor;
			int selectionID;
			bool visible;
			bool isTextured;
			bool isSprite;
			bool ScreenSpace;
			bool selectable;
			bool selected;
			bool debug;
			RenderSetting^ renderSettings;
			Mode mode;
			int debugCount;
			int debugIndex;

			void GenerateID()
			{
				selectionID = ObjectSelection::CreateColourID();
			}
			/*
			void GenerateID(float offset)
			{
				selectionID = ObjectSelection::CreateColourID();
			}
			*/

			void SetColour(Colour^ color)
			{
				colour = color;
				defaultColor = color;
			}

			virtual void Draw()
			{

			}

			virtual cli::array<Vector^>^ GetCollisionPoints()
			{
				return nullptr;
			}

			void LoadTexture(String^ imagePath)
			{
				texture = gcnew Texture(imagePath); //need to check is texture path is valid
				isTextured = true;
			}
		};
	}
}
