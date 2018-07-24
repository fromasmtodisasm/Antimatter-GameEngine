#pragma once
#include "SystemHeader.h" //<- prevent win.h/c#form conflicts
#include "GlaxionHeader.h"
#include "FontManager.h"
#include "glaxRender2D.h"
#include "Vector.h"
#include "Colour.h"
#include "Transform.h"
#include "Graphic.h"
#include "glaxVAO.h"
#include "RenderSettings.h"
#include "LineGraphic.h"
#include "TextGraphic.h"

//ray triangle intersection
//http://www.3dcpptutorials.sk/index.php?id=51
//ndc to world
//http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/

namespace Antimatter {
	namespace Games {
		public ref class Draw
		{
		public:
			Draw()
			{
				gxRender = glx::Render::Default;
				gxRender2D = &gxRender->render2d;
			};

			glx::Render* gxRender;
			glx::Render2D* gxRender2D;
			static Draw^ draw = gcnew Draw();

			static Colour^ ReadPixel(float x, float y)
			{
				float values[4] = { };
				glReadPixels(x, glx::Core::windowHeight - y, 1, 1, GL_RGBA, GL_FLOAT, values);
				return gcnew Colour(values[0], values[1], values[2], values[3]);
			}

			static glm::vec2 DrawDeviceCoord(float x, float y,float w)
			{
				glm::vec2 screenPos;
				screenPos.x = x / w / 2.0f + 0.5f;
				screenPos.y = -y / w / 2.0f + 0.5f;
				return screenPos;
			}

			static void Mesh(Transform^ transform,glx::VAO* vao,RenderSetting^ renderSettings,Colour^ colour,int textureid) 
			{
				//todo:  compare pointer params to values
				draw->gxRender->DrawMesh(*vao, transform->gxtransform->GetWorldMatrix(), colour->glm(), renderSettings->ToGLX(), textureid);
			}

			static void Triangle(Vector^ a, Vector^ b, Vector^ c,Colour^ col)
			{
				Draw::DebugLine(a, b, col);
				Draw::DebugLine(b, c, col);
				Draw::DebugLine(c, a, col);
			}

			//static void 
			static void Line(Vector^ a, Vector^ b, Colour^ col, bool ontop, bool orthographic)
			{
				draw->gxRender->DrawLine(a->glm(), b->glm(), col->glm(), col->glm(), 1.f, ontop, orthographic);
			}

			static void Text(float x, float y, ...cli::array<String^>^ text)
			{
				Text(x, y, Colour::White, 1.f, 1.f, text);
			}

			static void Text(float x, float y, Colour^ colour, ...cli::array<String^>^ text)
			{
				Text(x, y, colour, 1, 1, text);
			}

			static void Text(float x, float y, Colour^ colour, float scale, ...cli::array<String^>^ text)
			{
				Text(x, y, colour, scale, scale, text);
			}

			static void Text(float x, float y, float scaleX,float scaleY, ...cli::array<String^>^ text)
			{
				Text(x, y, Colour::White, scaleX, scaleY, text);
			}

			static void Text(float x,float y,Colour^ colour,float scaleX, float scaleY,...cli::array<String^>^ text)
			{
				for (int i = 0; i < text->Length; i++)
				{
					RenderString(text[i], x, y-((i*scaleY)/10.f), colour, scaleX, scaleY);
				}
			}

			static void RenderString(String^ text, float x, float y, Colour^ colour, float scalex, float scaley)
			{
				gcnew ScreenText(text, colour, x, y, scalex, scaley);
			}

			static void TransformedText(String^ text, Vector^ pos, Colour^ colour, Transform^ transform, float scalex, float scaley)
			{
				FontManager::DrawInWorld(text, pos->glm(), colour, scalex, scaley, transform->gxtransform->GetWorldMatrix(), false);
			}

			static void TransformBillboard(String^ text, Transform^ t, Vector^ position, Colour^ colour, float scalex, float scaley)
			{
				TextInWorld(text, t*position->ToGLM4(1), colour, scalex, scaley, true);
			}

			static void DrawAxis(Transform^ t)
			{
				Vector^ pos = t->Position;
				DebugVector(pos, t*Vector::right,Colour::Purple);
				DebugVector(pos, t*Vector::up, Vector::up->AsColour());
				DebugVector(pos, t*Vector::front, Colour::Blue);
			}

			static void TextInWorld(String^ text,Vector^ pos, Colour^ colour, float scalex, float scaley,bool billboard)
			{
				if (text == nullptr)
					text = "";
				gcnew WorldText(text, colour, pos, scalex, scaley,billboard);
			}

			static void TextBillboardInWorld(String^ text, Vector^ pos, Colour^ colour, float scalex, float scaley)
			{
				TextInWorld(text, pos, colour, scalex, scaley, true);
			}
			static void TextBillboardInWorld(String^ text, Vector^ pos, Colour^ colour,float scale)
			{
				TextInWorld(text, pos, colour, scale, scale, true);
			}

			static void DebugLine(Vector^ a, Vector^ b, Colour^ col)
			{
				gcnew LineGraphic(a, b, col, col, true);
			}

			static void DebugLine(Vector^ a, Vector^ b, Colour^ col1,Colour^ col2)
			{
				gcnew LineGraphic(a, b, col1, col2, true);
			}

			static void DebugLine(Vector^ a, Vector^ b, Colour^ col1, Colour^ col2,float size)
			{
				gcnew LineGraphic(a, b, col1, col2, true);
			}

			static void DebugVector(Vector^ a, Vector^ b, Colour^ col, String^ text,float scale)
			{
				Vector^ offset = a + b;
				DebugVector(a, b, col);
				TextBillboardInWorld(text, offset, Colour::White, scale);
			}

			static void ListIndices(List<Vector^>^ list,Colour^ col, float scale)
			{
				for (int i = 0; i < list->Count; i++)
					TextBillboardInWorld(i.ToString(), list[i], col, scale);
			}

			static void ShowAxis(Transform^ t, bool gimbal, bool direction)
			{
				
				if (gimbal)
					DrawAxis(t);

				if (direction)
					DisplayAxis(t);
			}

			static void DisplayAxis(Transform ^ t)
			{
				TransformBillboard("Right", t, Vector::right, Colour::Purple, 1, 1);
				TransformBillboard("Up", t, Vector::up, Colour::Green, 1, 1);
				TransformBillboard("Front", t, Vector::front, Colour::Blue, 1, 1);
			}

			static void Line(Vector^ a, Vector^ b, Colour^ col)
			{
				gcnew LineGraphic(a, b, col, col,false);
			}

			static void DebugLine(Vector^ a, Vector^ b, Colour^ col, Transform^ t)
			{
				gcnew LineGraphic(t*a, t*b, col, true);
			}

			static void DebugVector(Vector^ a, Vector^ b, Colour^ col)
			{
				gcnew LineGraphic(a, b, col, col, true);
			}

			static void DebugVector(Vector^ a, Vector^ b, Colour^ col,String^ text,Colour^ textCol,float textScale)
			{
				gcnew LineGraphic(a, a+b, col, col,true);
				gcnew WorldText(text, textCol, a+b, textScale, textScale, true);
			}

			static void DebugVector(Vector^ a, Vector^ b, Colour^ col,Transform^ t)
			{
				gcnew LineGraphic(t*a->W(1.f), t*(a+b)->W(1.f), col, col, true);
			}

			static void DebugVector(Vector^ a, Colour^ col,Vector^ b, Colour^ col2, Transform^ t)
			{
				gcnew LineGraphic(t*a->W(1.f), t*(a+b)->W(1.f), col, col2, true);
			}
			
			static void DebugPointRaw(Vector^ p, Colour^ col, float scale)
			{
				DebugLine(p+(Vector::down*scale), p+(Vector::up*scale), col);
				DebugLine(p+(Vector::right*scale), p+(Vector::left*scale), col);
				DebugLine(p+(Vector::front*scale), p+(Vector::back*scale), col);
			}
			static void DebugPointRaw2D(Vector^ p, Colour^ col, float scale)
			{
				DebugLine(p + (Vector::down*scale), p + (Vector::up*scale), col);
				DebugLine(p + (Vector::right*scale), p + (Vector::left*scale), col);
			}

			static void DebugPointRaw2DOrtho(Vector^ p, Colour^ col, float scale)
			{
				Vector^ a1 = p + (Vector::down*scale);
				Vector^ a2 = p + (Vector::up*scale);

				Vector^ b1 = p + (Vector::right*scale);
				Vector^ b2 = p + (Vector::left*scale);

				LineGraphic^ l1 = gcnew LineGraphic(a1, a2, col, col, true);
				LineGraphic^ l2 = gcnew LineGraphic(b1, b2, col, col, true);

				l1->orthoGraphic = true;
				l2->orthoGraphic = true;
			}

			static void DrawPolygon2D(Vector^ a,Colour^ colourA,Vector^ b, Colour^ colourB,Vector^ c, Colour^ colourC,bool enableLighting)
			{
				//draw->gxRender2D->TestDrawTriangle(0.f, 0.f, glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec2(1.f,1.f));
				//watch bool to int conversion
				draw->gxRender2D->DrawTriangle(a->ToGLM2(),colourA->glm(),b->ToGLM2(),colourB->glm(),c->ToGLM2(),colourC->glm(),enableLighting);
			}
			static void DrawTriangle2D(Vector^ position, Colour^ colourA, Colour^ colourB, Colour^ colourC, float scale,bool enableLighting)
			{
				
				Vector^ A = gcnew Vector(-1, 1,0)*scale+position;

				Vector^ B = gcnew Vector(1, 1,0)*scale+position;

				Vector^ C = gcnew Vector(0, -1,0)*scale+position;
				//colA = gcnew Colour(player->position->ToGLM4());
				Draw::DrawPolygon2D(A, colourA, B, colourB, C, colourC, true);
			}

			static void Set2DLightPosition(Vector^ position)
			{
				draw->gxRender2D->lightPosition = position->ToGLM3();
			}
			
			static void Diamond2D(Vector^ position,Colour^ colour,float scale,float glow)
			{
				Colour^ g =gcnew Colour(colour,glow);
				draw->gxRender2D->DrawDiamond(position->x, position->y, colour->glm(), g->glm(), scale);
			}

			static void Diamond2D(Vector^ position, Colour^ colour, float scale, Colour^ glow)
			{
				draw->gxRender2D->DrawDiamond(position->x, position->y, colour->glm(), glow->glm(), scale);
			}

			static void Line2D(Vector^ A,Vector^ B, Colour^ colourA, Colour^ colourB, float thickness, bool enableLighting)
			{
				glm::vec4 colA = colourA->glm();
				glm::vec4 colB = colourB->glm();
				glm::vec2 a = A->ToGLM2();
				glm::vec2 b = B->ToGLM2();
				//watch bool to int conversion
				draw->gxRender2D->DrawLine(a,colA, b,colB, thickness, 0);
				//draw->gxRender2D->DrawTriangle(left, col, bottom, col, right, col, enableLighting);
			}

			static void Diamond2D(float x, float y, Colour^ colour, float scale,bool enableLighting)
			{
				/*
				glm::vec4 col = colour->glm();
				glm::vec3 middle = glm::vec3(x, y, 0.f);
				glm::vec3 bottom = (middle + Vector::down)*scale;
				glm::vec3 top = (middle + Vector::up)*scale;
				glm::vec3 left = (middle + Vector::left)*scale;
				glm::vec3 right = (middle + Vector::right)*scale;
				//Draw::DebugPointRaw2D(gcnew Vector(middle*scale), Colour::White, 1.f);
				*/
				glm::vec4 glow = glm::vec4(0.f);
				if (colour->a > 1.f)
					glow = colour->glm();
				draw->gxRender2D->DrawDiamond(x,y, colour->glm(), glow, scale);
				//watch bool to int conversion
			}

			static void Buffers2D()
			{
				draw->gxRender2D->DrawBuffers();
			}

			static void Start2DGlowBuffer()
			{
				draw->gxRender2D->StartMultiBufferDraw();
				draw->gxRender2D->EnableDualBufferDraw();
			}

			static void EnableGlowBuffer()
			{
				draw->gxRender2D->EnableDualBufferDraw();
			}

			static void DisableGlowBuffer()
			{
				draw->gxRender2D->EnableSingleBufferDraw();
			}

			static Colour^ ReadPixel2D(float X, float Y)
			{
				Colour^ c = gcnew Colour(draw->gxRender2D->ReadPixel(X,Y));
				return c;

			}
		};
	}
}

