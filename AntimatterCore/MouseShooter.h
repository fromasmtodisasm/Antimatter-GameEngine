#pragma once
#include "Scene.h"
#include "Sound.h"

namespace Antimatter {
	namespace Games {

		public ref class MouseShooter : Scene
		{
		public:

			MouseShooter();
			void BlastNodes(Vector ^ point);
			void DrawNodes(Colour^ colour);
			void LoadPointGrid();
			virtual void Update() override;
			void DrawWeb(Colour ^ col);
			Colour^ colA = Colour::LightBlue->Alpha(0.f);
			Colour^ colB = Colour::Blue->Alpha(1.5f);
			Colour^ colC = Colour::Black->Alpha(0.f);
			bool enbaleGridLighting = true;
			cli::array<int,2>^ stock = gcnew cli::array<int,2>(20,20);
			cli::array<Colour^, 2>^ stockColour = gcnew cli::array<Colour^, 2>(20, 20);

			Sound^ music1;
			int skipdraw = 0;

			ref class Node
			{
			public:
				Node()
				{
					velocity = Vector::Zero;
					force = Vector::Zero;
					lerp = 0.f;
				}

				int i;
				int j;
				Vector^ pos;
				Colour^ col;
				Colour^ glow;
				Vector^ force;
				Vector^ startPos;
				Vector^ velocity;
				float mass;
				float lerp;

				void Update()
				{
					velocity += force;
					pos += velocity;
					force = Vector::Zero;
					//pos += (pos - startPos)* 0.05f;
					lerp += 0.001f;
					pos = Vector::Lerp(startPos,pos,lerp);
					float dist = Vector::Distance(pos, startPos);
					if (dist > 5.f)
						velocity = startPos - pos;
					velocity *= dist;
					
					///if (lerp > 1.f)
					//	lerp = 1.f;
					
				}
			};
			List<Node^>^ nodes = gcnew List<Node^>();
			List<Node^>^ web = gcnew List<Node^>();

			ref class GLine
			{
			public:
				GLine(Vector^ posA, Vector^ posB, Colour^ Col, float Width)
				{
					a = posA;
					b = posB;
					colour = Col;
					width = Width;

				}
				Vector^ a;
				Vector^ b;
				Colour^ colour;
				float width;

				void Draw()
				{
					Draw::Line2D(a, b, colour, colour, width, false);
				}
			};
			List<GLine^>^ glines = gcnew List<GLine^>();

			ref class Player : Transform2D
			{
			public:
				Player() :Transform2D()
				{
					scale = 0.1f;
					colour = Colour::Purple;
					defaultColour = colour;
					lighting = false;
					projectionType = ProjectionType::Distance;
				}
				Colour^ colour;
				Colour^ defaultColour;
				float scale;
				float power;
				bool lighting;
				enum class ProjectionType {Distance,DotProduct};
				ProjectionType projectionType;

				void Draw()
				{
					//Draw::Text(-1.f, -0.7f, position->x.ToString(), position->y.ToString());
					//Draw::DrawTriangle2D(position,colour,colour,colour,0.1f,false);
					Draw::Diamond2D(position, Colour::New, 0.02f, 1.2f);
				}
			};

			Player^ player;
			
		};
	}
}

