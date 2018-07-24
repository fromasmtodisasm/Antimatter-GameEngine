#pragma once
#include "SystemHeader.h" //<- prevent win.h/form conflict
#include "GlaxionHeader.h"

namespace Antimatter {
	namespace Games {

		public ref class Graphic
		{
		public:

			Graphic::Graphic()
			{
				if (!FreezeGraphics)
				Batches->Add(this);
			}
			int lifeTime = -1;
			float width;
			bool overrideBatchRender;
			static List<Graphic^>^ Batches = gcnew List<Graphic^>();
			static bool FreezeGraphics;

			virtual void Graphic::Update()
			{

			}

			virtual void Graphic::Draw()
			{

			}

			static void UpdateBatches()
			{
				for each(Graphic^ g in Batches)
				{
					if(!g->overrideBatchRender)
					g->Update();
				}
			}
			
			static void DrawBatches()
			{
				List<Graphic^>^ gl = gcnew List<Graphic^>();
				for each(Graphic^ g in Batches)
				{
					if (g->overrideBatchRender)
						continue;
					g->Draw();
					if (!FreezeGraphics)
					{
						--g->lifeTime;
						if (g->lifeTime < 0)
						{
							gl->Add(g);
						}
					}
				}
				if (!FreezeGraphics)
				{
					for each(Graphic^ g in gl)
					{
						Batches->Remove(g);
						g = nullptr;
					}
					gl->Clear();
				}
			}
		};
	}
}

