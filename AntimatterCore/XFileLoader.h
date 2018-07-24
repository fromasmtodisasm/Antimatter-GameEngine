#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Vector.h"
#include "SubGeometry.h"

using namespace System::Text::RegularExpressions;
using namespace System::Globalization;
using namespace System::Diagnostics;

namespace Antimatter {
	namespace Games {

		public ref class TextureUV
		{
		public:
			TextureUV()
			{
				U = 0;
				V = 0;
			}

			TextureUV(float u, float v)
			{
				U = u;
				V = v;
			}

			float U;
			float V;
		};

		public ref class XFileLoader
		{
		public:
			XFileLoader()
			{

			};

			XFileLoader(String^ path, bool debug)
			{
				LoadMesh(path, debug);
			};

			unsigned int bufferID;
			unsigned int dataCount;
			bool loaded;
			bool debug=false;
			float Frame;
			int bufferPosID;
			String^ name;
			String^ filePath;
			Regex^ regexInt;
			List<float>^ vertexData = gcnew List<float>();
			List<unsigned int>^ indexList = gcnew List<unsigned int>();
			List<Vector^>^ normals = gcnew List<Vector^>();
			List<TextureUV^>^ UVs = gcnew List<TextureUV^>();
			List<Vector^>^ faceNormals = gcnew List<Vector^>();
			List<Face^>^ faces = gcnew List<Face^>();
			List<Vertex^>^ vertices = gcnew List<Vertex^>();
			
			unsigned int GetDataSize()
			{
				unsigned int byteSize = (unsigned int)vertexData->Count * sizeof(float);
				return byteSize;
			}

			void CreateVertexNormals()
			{
				if (debug)
				{
					Console::WriteLine("Face count: " + faces->Count);
					Console::WriteLine("Face Normal count: " + faceNormals->Count);
				}

				for (int i = 0; i < vertexData->Count; i++)
				{
					float x = 0, y = 0, z = 0;
					int num = 0;
					for (int j = 0; j < faces->Count; j++)
					{
						if (faces[j]->A == i || faces[j]->B == i || faces[j]->C == i)
						{
							x += faceNormals[j]->x;
							y += faceNormals[j]->y;
							z += faceNormals[j]->z;
							num++;
						}
					}

					if (num != 0)
					{
						x /= num;
						y /= num;
						z /= num;
					}

					float d = (float)Math::Sqrt(x*x + y*y + z*z);
					if (d != 0)
					{
						x /= d;
						y /= d;
						z /= d;
					}
					normals->Add(gcnew Vector(x, y, z));
				}
			}

			cli::array<float>^ BuildInterleaveData()
			{
				if (vertexData->Count == 0)
				{
					if(debug)
					tool::show(name+" error, no vertex data");
					return nullptr;
				}

				if (normals->Count == 0)
				{
					if(debug)
					tool::show(name + " XFile:: GetMesh:" + name + " error, no normal data");
					return nullptr;
				}

				if (UVs->Count == 0)
				{
					if(debug)
					tool::debug("XFile:: GetMesh:" + name+" no UV data");
				}
				else
				{
					if(debug)
					tool::debug("XFile:: GetMesh:" + name + " UVs " + UVs->Count.ToString());
				}

				List<float>^ dat = gcnew List<float>();
				int vertIndex = 0;

				if (debug)
				{
					tool::debug(" interleave vertex count: " + vertexData->Count.ToString(), " interleave normal count: " + normals->Count.ToString());
				}

				for (int i = 0; i < vertexData->Count / 3; i++)
				{
					dat->Add(vertexData[vertIndex]);
					dat->Add(vertexData[vertIndex + 1]);
					dat->Add(vertexData[vertIndex + 2]);
					dat->Add(normals[i]->x);
					dat->Add(normals[i]->y);
					dat->Add(normals[i]->z);

					if (UVs->Count > 0)
					{
						dat->Add(UVs[i]->U);
						dat->Add(UVs[i]->V);
					}
					else
					{
						dat->Add(0.5f);
						dat->Add(0.5f);
					}
					vertIndex = vertIndex + 3;
				}

				if (vertexData->Count != normals->Count)
				{
					tool::debug(filePath + "\nerror, vertex count and normal count do not match" + "\n\nData count: " + vertexData->Count.ToString() + "\nNormal count: " + indexList->Count.ToString());
				}
				return dat->ToArray();
			}

			void TestInterleave()
			{
				if (normals->Count == 0)
				{
					CreateVertexNormals();
				}

				cli::array<float>^ dat = BuildInterleaveData();

				if (debug)
					tool::debug("test interleave data: " + normals->Count);

				int verti = 0;
				int num = 0;
				for (int i = 0; i < dat->Length / 6; i++)
				{
					if (debug)
					{
						tool::debug("\nVertices: " + dat[verti] + "," + dat[verti + 1] + "," + dat[verti + 2]);
						tool::debug("\nnormal: " + dat[verti + 3] + "," + dat[verti + 4] + "," + dat[verti + 5] + "\n");
					}
					verti += 6;
					num += 6;
				}
				if (debug)
					tool::debug("\n verti: " + num);
			}

			void LoadMesh(String^ filePath, bool debug);
		};
	}
}