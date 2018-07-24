#pragma once
#include "glaxCore.h"

namespace Antimatter
{
	namespace glx
	{
		class VAO
		{
		public:
			VAO()
			{
				ID = 0;
				buffer = 0;
				indexLength = 0;
				indexPosition = 0;
				bufferID = 0;
			};

			unsigned int ID;
			unsigned int buffer;
			unsigned int bufferID;
			unsigned int indexLength;
			unsigned int indexPosition;

			void debugMe(const char* text)
			{
				cout << "\nVAO: debugMe: " << text << endl;
				cout << "-> ID: " << ID << endl;
				cout << "-> buffer number: " << buffer << endl;
				cout << "-> index length: " << indexLength << endl;
				cout << "-> index position: " << indexPosition << endl;
				cout << endl;
			}

			void DrawMesh()
			{
				glx::Core::SelectBuffer(bufferID);
				glBindVertexArray(ID);
				glDrawElements(GL_TRIANGLES, indexLength, GL_UNSIGNED_INT, (char*)NULL + (sizeof(unsigned int)*indexPosition));
				glBindVertexArray(0);
			}
		};
	}
}