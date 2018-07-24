#pragma once
#include "SystemHeader.h"

using namespace std;

namespace Antimatter
{
	namespace glx
	{
		template <typename T>
		static void DebugObject(const char* s, const T& object)
		{
			cout << "\n-> > " << s << ": " << object << "<-"<<endl;
		}

		static void  DebugString(const char* s)
		{
			cout<< "\n-> > " << s << " <-"<<endl;
		}

		static void Print(glm::vec3 vec3)
		{
			cout << " Debug glm::vec3: " << endl;
			cout << "X: " << vec3.x << endl;
			cout << "Y: " << vec3.y << endl;
			cout << "Z: " << vec3.z << endl;
			cout << endl;
		}

		static void Print(glm::vec3 vec3,const char * text)
		{
			cout << " Debug glm::vec3: " << text<<endl;
			cout << "X: " << vec3.x << endl;
			cout << "Y: " << vec3.y << endl;
			cout << "Z: " << vec3.z << endl;
			cout << endl;
		}

		static void Print(glm::vec4 vec4)
		{
			cout << " Debug glm::vec4: " << endl;
			cout << "X: " << vec4.x << endl;
			cout << "Y: " << vec4.y << endl;
			cout << "Z: " << vec4.z << endl;
			cout << "W: " << vec4.w << endl;
			cout << endl;
		}

		static void Print(glm::vec4 vec4,const char * text)
		{
			cout << " Debug glm::vec4: " << text<<endl;
			cout << "X: " << vec4.x << endl;
			cout << "Y: " << vec4.y << endl;
			cout << "Z: " << vec4.z << endl;
			cout << "W: " << vec4.w << endl;
			cout << endl;
		}
	}
}

