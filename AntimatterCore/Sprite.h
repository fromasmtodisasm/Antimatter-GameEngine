#pragma once
#include "Thing.h"
#include "Transform2D.h"

namespace Antimatter {
	namespace Games {

		public ref class Sprite : public Thing
		{
		private:
			void Construction()
			{
				name = "Sprite";
				transform2D = gcnew Transform2D();
				SetColour(gcnew Colour(1.f, 1.f, 1.f, 1.f));
				isSprite = true;
				renderSettings->disableCulling = true;
				selectable = true;
				vao = glx::Render::DefaultSprite;
			}

		public:
			Sprite() :Thing()
			{
				Construction();
			};

			glx::VAO* vao;
			Transform2D^ transform2D;
			bool billboard = false;

			Sprite(String^ imagePath) : Thing()
			{
				Construction();
				AddTexture(imagePath);
			}

			void AddTexture(String^ imagePath)
			{
				if (tool::FilePathHasInvalidChars(imagePath))
				{
					tool::show("invalid characters in path...", imagePath);
					return;
				}
				texture = gcnew Texture(imagePath);
				name = Path::GetFileNameWithoutExtension(imagePath);
			}

			void Draw() override
			{
				//TODO wrap static renderer with static functions
				if (!visible)
					return;

				Colour^ col = nullptr;
				glx::Render* renderer = glx::Render::Default;
				if (colour != nullptr)
				{
					col = colour;
				}
				else {
					col = defaultColor;
				}

				glm::mat4 trans = gxtransform->GetWorldMatrix();
				//load default shape of sprite
				renderer->DrawSprite(*glx::Render::DefaultSprite, trans, col->glm(), texture->id, renderSettings->ToGLX(), billboard);
			}
		};
	}
}

