#pragma once
#include "SystemHeader.h"

namespace Antimatter
{
	namespace glx
	{
		struct Primtive
		{
		public:
			Primtive(){}

			//copy constructor
			Primtive(const Primtive& vbo)
			{
				VAO = vbo.VAO;
				IndexLength = vbo.IndexLength;
				IndexOffset = vbo.IndexOffset;
				buffered = vbo.buffered;
				
			}

			const char * filePath;
			unsigned int VAO = 0;
			unsigned int IndexOffset = 0;
			unsigned int IndexLength = 0;
			bool buffered = false;
			~Primtive(){};

			void Debug(const char * text)
			{
				cout << "\n" << text << " VBO VAO: " << VAO<<endl;
				cout << text << " VBO index offset: " << IndexOffset<<endl;
				cout << text << " VBO index length: " << IndexLength<<"\n"<<endl;
			}
		};
	}
}