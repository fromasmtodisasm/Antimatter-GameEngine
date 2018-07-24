#pragma once

//#include "stdafx.h"
#include "Windows.h"
#define GLEW_STATIC
#include <glew.h>
#include <GLFW\glfw3.h>
//#include "GLUT\freeglut.h" //used for wglcontext calls and gluGetErrorString
#include <glm\glm.hpp>
#include <glm\mat4x4.hpp>
#include <glm\mat3x3.hpp>
#include <glm\trigonometric.hpp>
#include <glm\gtx\projection.hpp>
#include <glm\matrix.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\quaternion.hpp>
#include <iostream>
#include "SDKDDKVer.h"  //<- defs for latest window being used
#include <stdio.h>
#include <tchar.h>
#include <shellapi.h>
#include <vector>
#include <list>
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <iterator>
#include <string>
#include <math.h>
#include <fstream>
#include "Definitions.h"