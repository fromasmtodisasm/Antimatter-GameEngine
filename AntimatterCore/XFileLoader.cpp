#include "XFileLoader.h"

using namespace Antimatter::Games;

void XFileLoader::LoadMesh(String^ File_Path, bool debug)
{
	filePath = File_Path;

	bool beginNormalRead = false;
	bool beginVertRead = false;
	bool beginIndexRead = false;
	bool beginUVRead = false;
	Stream^ stream = File::OpenRead(filePath);
	StreamReader^ sr = gcnew StreamReader(stream);
	String^ line;
	String^ findnumber = "([0-9]+)";
	int index = 0;
	int vertexIndex = 0;

	regexInt = gcnew Regex(findnumber, System::Text::RegularExpressions::RegexOptions::CultureInvariant);
	Regex^ parseMesh = gcnew Regex("Mesh {");
	Regex^ parseNormals = gcnew Regex("MeshNormals {");
	Regex^ parseUVs = gcnew Regex("MeshTextureCoords {");
	Regex^ end = gcnew Regex(";;");
	vertices->Clear();
	List<unsigned int>^ indices = gcnew List<unsigned int>();
	List<unsigned int>^ uvs = gcnew List<unsigned int>();
	
	while ((line = sr->ReadLine()) != nullptr)
	{
		if (parseMesh->IsMatch(line))
		{
			beginVertRead = true;
			if (debug)
				tool::debug(line);
		}

		if (parseNormals->IsMatch(line))
		{
			beginNormalRead = true;
			if (debug)
				tool::debug(line);
		}

		if (parseUVs->IsMatch(line))
		{
			beginUVRead = true;
			if (debug)
				tool::debug(line);
		}

		if (beginVertRead)
		{
			if (index == 1)
			{
				if (debug)
				{
					String^ resultString = regexInt->Match(line)->Value;
					int result = Int32::Parse(resultString);
					tool::debug("Vertex Count: " + result.ToString());
				}
			}

			//read position data
			if (index > 1)
			{
				cli::array<String^>^ parts = line->Split(';');
				float pos1 = float::Parse(parts[0]);
				float pos2 = float::Parse(parts[1]);
				float pos3 = float::Parse(parts[2]);
				vertexData->Add(pos1);
				vertexData->Add(pos2);
				vertexData->Add(pos3);
				if (debug)
					cout << "vertex: " << "X: " << pos1 << " Y: " << pos2 << " Z: " << pos3 << endl;
				Vertex^ v = gcnew Vertex(pos1, pos2, pos3);
				v->index = vertexIndex;
				vertexIndex++;
				vertices->Add(v);
			}
			index++;

			if (index > 1 && end->IsMatch(line))
			{
				beginIndexRead = true;
				if (debug)
					tool::debug("Begin Index read");
				beginVertRead = false;
				index = 0;
			}
		}

		if (beginIndexRead)
		{
			if (index == 1)
			{
				String^ resultString = regexInt->Match(line)->Value;
				int result = Int32::Parse(resultString);
				if (debug)
					tool::debug("Index Count: " + result.ToString());
			}

			if (index > 1)
			{
				cli::array<String^>^ parts = line->Split(';');
				cli::array<String^>^ ints = parts[1]->Split(',');
				ints[2]->Replace(" ", String::Empty);
				unsigned int ind1 = UINT::Parse(regexInt->Match(ints[0])->Value);
				unsigned int ind2 = UINT::Parse(regexInt->Match(ints[1])->Value);
				unsigned int ind3 = UINT::Parse(regexInt->Match(ints[2])->Value);
				if (debug)
					tool::debug("Xparser: Debug Indices: " + ind1 + " " + ind2 + " " + ind3);
				indexList->Add(ind1);
				indexList->Add(ind2);
				indexList->Add(ind3);
				faces->Add(gcnew Face(ind1, ind2, ind3));
			}

			//End index read
			if (index > 1 && end->IsMatch(line))
			{
				indexList->Add(indexList[0]);
				if (debug)
					tool::debug("End index read");
				beginIndexRead = false;
				index = 0;
			}
			index++;
		}

		if (beginNormalRead)
		{
			if (index == 2)
			{
				//get the header data count
				if (debug)
				{
					String^ resultString = regexInt->Match(line)->Value;
					if (resultString != nullptr)
					{
						int result = Int32::Parse(resultString);
						tool::debug("Normal Count: " + result.ToString());
					}
				}
			}

			//read normal data
			if (index > 2)
			{
				cli::array<String^>^ parts = line->Split(';');
				float pos1 = float::Parse(parts[0]);
				float pos2 = float::Parse(parts[1]);
				float pos3 = float::Parse(parts[2]);
				if (debug)
					tool::debug(pos1 + " " + pos2 + " " + pos3);
				faceNormals->Add(gcnew Vector(pos1, pos2, pos3));
			}

			index++;

			if (index > 1 && end->IsMatch(line))
			{
				if (debug)
					tool::debug("End Normal read");
				beginNormalRead = false;
				index = 0;
				line = nullptr;
			}
		}

		if (beginUVRead)
		{
			if (index == 1)
			{
				//get the header data count
				if (debug)
					tool::debug("begin UV read");

				if (debug)
				{
					String^ resultString = regexInt->Match(line)->Value;
					if (resultString != nullptr)
					{
						int result = Int32::Parse(resultString);
						tool::debug("UV Count: " + result.ToString());
					}
				}
			}

			if (index > 1)
			{
				cli::array<String^>^ parts = line->Split(';');
				float pos1 = float::Parse(parts[0]);
				float pos2 = float::Parse(parts[1]);
				if (debug)
					tool::debug(String::Concat("U: ", pos1, " V: ", pos2));
				UVs->Add(gcnew TextureUV(pos1, pos2));
			}

			index++;

			if (index > 1 && end->IsMatch(line))
			{
				if (debug)
					tool::debug("End UV read");
				beginUVRead = false;
				index = 0;
				line = nullptr;
			}
		}
	}

	sr->Close();
	stream->Close();
	CreateVertexNormals();
	name = Path::GetFileNameWithoutExtension(filePath);
	if (debug)
		tool::debug(name);
	return;
}