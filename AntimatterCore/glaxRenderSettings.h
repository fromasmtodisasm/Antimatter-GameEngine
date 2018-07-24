#pragma once
#include "SystemHeader.h"

namespace Antimatter{
	namespace glx{

		struct RenderSetting
		{
		public:
			RenderSetting()
			{
				name = "";
				textureID = 0;
				lightingEnabled = true;
				screenSpace = false;
				wireFrame = false;
				solid = true;
				points = false;
				disableCulling = false;
				backFaceCulling = false;
				frontfaceCulling = false;
				onTop = false;
				_selectable = false;
				is2D = false;
			}

			//copy constructor
			RenderSetting(const RenderSetting& setting)
			{
				name = setting.name;
				textureID = setting.textureID;
				lightingEnabled = setting.lightingEnabled;
				screenSpace = setting.screenSpace;
				solid = setting.solid;
				points = setting.points;
				wireFrame = setting.wireFrame;
				disableCulling = setting.disableCulling;
				backFaceCulling = setting.backFaceCulling;
				frontfaceCulling = setting.frontfaceCulling;
				onTop = setting.onTop;
				_selectable = setting._selectable;
				is2D = setting.is2D;
			}

			char* name;
			int textureID;
			bool lightingEnabled;
			bool screenSpace;
			bool wireFrame;
			bool solid;
			bool points;
			bool disableCulling;
			bool backFaceCulling;
			bool frontfaceCulling;
			bool onTop;
			bool _selectable;
			bool is2D;

			~RenderSetting() {};

			void Debug(const char * text)
			{
				cout << "\n" << text << " name: " << name;
				cout << "\n" << text << " texture iD: " << textureID;
				cout << "\n" << text << " lighting enabled: " << lightingEnabled;
				cout << "\n" << text << " screen space: " << screenSpace;
				cout << "\n" << text << " wireframe: " << wireFrame;
				cout << "\n" << text << " soild: " << solid;
				cout << "\n" << text << " points: " << points<<endl;
				cout << "\n" << text << " disable culling " << disableCulling << endl;
				cout << "\n" << text << " backface culling: " << backFaceCulling << endl;
				cout << "\n" << text << " front face culling" << frontfaceCulling << endl;
				cout << "\n" << text << " Draw in front" << onTop << endl;
				cout << "\n" << text << " is 2d" << is2D<< endl;
			}
		};
	}
}