#include "MouseShooter.h"

using namespace Antimatter::Games;

MouseShooter::MouseShooter() : Scene()
{
	player = gcnew Player();
	music1 = gcnew Sound("Assets\\Mind Sweeper Fog.mp3");
	keyboard = Keyboard::defaultKeyboard;
	LoadPointGrid();
	//music1->mute = true;
}


//TODO//implement glow
/*
-colour values above 1 and considered "emissive light"
-shader or cpu can sort the above colour and render the resulting light volumes into a light buffer
-Need to split the render targets at this point.  See the references in DeferedShader.h
-Disable glDrawBuffers to revert back to normal render state
-there should be an FBO that looks like the scene with no lights.  Take this
-and combine it the light buffer FBO.  Apply gaussin blur to the textures
//
*/
void MouseShooter::Update() 
{
	static bool startMusic = false;
	
	if (startMusic == false)
	{
		music1->PlayLoop(1.f);
		startMusic = true;
	}
	
	PrepareScene();
	Draw::Start2DGlowBuffer();

	Vector^ mp = editorMouse->mouse->position;
	
	Vector^ A = gcnew Vector(-1, -1);

	Vector^ B = gcnew Vector(1, -1);
	
	Vector^ C = gcnew Vector(0, 1);

	Draw::DrawPolygon2D(A,colA,B,colB,C,colC,true);
	Draw::DrawTriangle2D(gcnew Vector(0.8f, 0.9f), Colour::LightBlue, Colour::LightBlue, Colour::LightBlue, 0.06f, true);
	
	//Draw::Text(0.8f, 0.9f, "X: 0.8f", "Y: 0.9f");
	if (editorMouse->mouse->IsReleased(MouseButton::Left))
	{
		player->colour->setRGB(Draw::ReadPixel2D(mp->x, mp->y));
	}

	player->position = editorMouse->mouse->GetNormalisedPostion();
	player->position->x *= 1.5f;
	
	Vector^ mndcp = player->position;
	Draw::Set2DLightPosition(player->position);

	for (int i = glines->Count - 1; i>0; i--)
	{
		glines[i]->Draw();
	}
	//Vector^ lazer = (Vector::Zero - player->position)->normalized();
	player->power = editorMouse->mouse->zoom*0.3f;
	//Draw::Line2D(lazer*2.f, player->position, player->colour->Alpha(1.2f), player->colour->Alpha(1.2f), player->power, false);
	if (editorMouse->mouse->IsPressed(MouseButton::Right))
	{
		BlastNodes(player->position);
		//GLine^ gline = gcnew GLine(lazer*2.f, player->position, player->colour->Alpha(1.2f), player->power);
		//glines->Add(gline);
	}

	if (keyboard->IsPressed(KeyButton::A))
	{
		music1->mute = !music1->mute;
		if(!music1->mute)
			music1->PlayLoop(1.f);
		else
			music1->PlayLoop(0.f);
	}

	
	DrawNodes(Colour::Yellow);
	player->Draw();
	DrawWeb(Colour::Green);
	Draw::DisableGlowBuffer();
	Graphic::DrawBatches();
	
	Draw::Buffers2D();
}

void MouseShooter::DrawWeb(Colour^ col)
{
	int count = web->Count;
	for (int i = 0; i < count; i++)
	{
		if (i == 0)
			continue;
		int j = i + 1;
		if (j >= count)
			return;
		Colour^ c = col->Alpha(1.1f);
		
		Draw::Line2D(web[i]->pos, web[j]->pos, c, c, 0.015f, false);
	}
}


void MouseShooter::BlastNodes(Vector^ point)
{

	for each(Node^ node in nodes)
	{
		Vector^ D = node->pos;
		float dist = Vector::Distance(D, point);
		if (dist < 0.3f*player->power && dist > 0.01f)
		{
			float scale = 0.3f*player->power - dist;
			Vector^ dir = (D - point)->normalized()*dist*0.3f;
			node->force += dir*0.5f;
			node->lerp = -.01f;
		}
	}
}

void MouseShooter::DrawNodes(Colour^ col)
{
	
	Vector^ pos = player->position;
	for each(Node^ node in nodes)
	{
		node->Update();
		Vector^ D = node->pos;
		float dist = Vector::Distance(D, pos);
		float dot = Vector::Dot(D, pos);
		float power = player->power;
		if (dist < 0.7f*player->power && dist > 0.01f)
		{
			float scale = 0.7f*player->power - dist;
			if (scale > 0.7f)
				scale = 0.7f;
			Draw::Diamond2D(node->pos, player->colour, 0.02f*scale, player->colour);
		}
		
		if (dist < 0.2f)
		{
			Colour^ c = gcnew Colour(player->colour, 1.4f);
			Draw::Line2D(node->pos, pos, Colour::New->Alpha(0.f), c, 0.1f, false);
			Draw::Diamond2D(node->pos, Colour::Yellow, 0.015f, 2.f);
			if (dist < 0.1f)
			{
				if(editorMouse->mouse->IsDown(MouseButton::Left))
				web->Add(node);
			}
		}
	}
}

void MouseShooter::LoadPointGrid()
{
	for (int i = -20; i < 20; i++)
	{
		for (int j = -20; j < 20; j++)
		{
			Node^ node = gcnew Node();
			node->pos = gcnew Vector(i, j)*0.12f;
			node->startPos = gcnew Vector(i, j)*0.12f;
			node->col = Colour::White->Alpha(0.f);
			node->i = i;
			node->j = j;
			nodes->Add(node);
		}
	}
}