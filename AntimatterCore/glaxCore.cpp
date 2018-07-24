#include "glaxCore.h"
//image loading
//https://www.imagemagick.org/script/magick++.php
//loading icon
//http://www.glfw.org/docs/latest/group__window.html#gadd7ccd39fe7a7d1f0904666ae5932dc5

namespace Antimatter {
	namespace glx {

		GLFWwindow*  glx::Core::glfwWindow = NULL;
		glx::Core* glx::Core::DefaultCore = NULL;
		int glx::Core::windowHeight = HEIGHT;
		int glx::Core::windowWidth = WIDTH;
		int glx::Core::smoothness = 8; //open gl only accepts multiples of 4
		float glx::Core::ratio = (float)WIDTH/(float)HEIGHT;
		int glx::Core::currentBuffer = 0;
		int glx::Core::currentShader = 0;
		bool glx::Core::trackOpenGLStates = false;
		bool glx::Core::Initialized = false;

		Core::Core()
		{
			std::vector<wchar_t> pathBuf;
			DWORD copied = 0;
			do {
				pathBuf.resize(pathBuf.size() + MAX_PATH);
				copied = GetModuleFileName(0, &pathBuf.at(0), pathBuf.size());
			} while (copied >= pathBuf.size());

			pathBuf.resize(copied);
			std::string path(pathBuf.begin(), pathBuf.end());
			Console::ForegroundColor = ConsoleColor::Yellow;
			cout << path;
			Console::ForegroundColor = ConsoleColor::White;

			if (!Core::Initialized)
			{
				Initialize();
				glx::Core::DefaultCore = this;
				Core::Initialized = true;
				return;
			}
		}

		void Core::SetCurrentContext(GLFWwindow* w)
		{
			glfwMakeContextCurrent(w);
			glfwSwapInterval(1);
		}

		void Core::Initialize()
		{
			if (!glfwInit())
			{
				cout << "FAILED to initialise GLFW" << endl;
				exit(EXIT_FAILURE);
			}

			glfwSetErrorCallback(&error_callback);
			glfwWindowHint(GLFW_SAMPLES, smoothness);  //multi sample buffers
			glfwWindowHint(GLFW_SRGB_CAPABLE, TRUE);
			PrintGLFWVersion();
			glx::Core::windowWidth = WIDTH;
			glx::Core::windowHeight = HEIGHT;
			glfwWindow = glfwCreateWindow(glx::Core::windowWidth, glx::Core::windowHeight, "Antimatter Engine", NULL, NULL);
			
			if (!glfwWindow)
			{
				glfwTerminate();
				exit(EXIT_FAILURE);
			}

			SetCurrentContext(glfwWindow);
			glfwSetKeyCallback(glfwWindow, key_callback);
			glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(glfwWindow, position_callback);
			glfwSetScrollCallback(glfwWindow, OnScroll);
			glfwSetMouseButtonCallback(glfwWindow, mouse_callback);
			glfwSetInputMode(glfwWindow, GLFW_STICKY_MOUSE_BUTTONS, 1);
			glfwSetWindowSizeCallback(glfwWindow, window_size_callback);
			glfwSetDropCallback(glfwWindow, drop_callback);
			glfwSetWindowUserPointer(glfwWindow, this);
			glfwSetWindowPos(glfwWindow, 500, 50);
			glfwSetWindowFocusCallback(glfwWindow, window_focus_callback);
			//initialize glew
			InitializeGLEW();
		}

		void Core::InitializeGLEW()
		{
			GLenum glewErr = glewInit();
			if (GLEW_OK != glewErr)
			{
				cout << "\nGlEW Error: " << glewErr << endl;
				return;
			}
			cout << "\nGLEW OK" << endl;
			//			while (GL_NO_ERROR != glGetError());

			//initial opengl state
			glShadeModel(GL_SMOOTH);  
			glEnable(GL_MULTISAMPLE);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glClearDepth(1.0f);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glEnable(GL_DEPTH_TEST);
			//glViewport(0, 0, Core::windowWidth, Core::windowHeight);
		}

		void Core::HideWindow()
		{
			glfwHideWindow(glfwWindow);
		}

		Core::~Core()
		{
			Close();
		}

		bool Core::SelectBuffer(int id)
		{
			if (currentBuffer != id)
			{
				glBindBuffer(GL_ARRAY_BUFFER, id);
				currentBuffer = id;
				return true;
			}
			return false;
		}

		bool Core::SelectShader(int id)
		{
			if (currentShader != id)
			{
				glUseProgram(id);
				currentShader = id;
				return true;
			}
			return false;
		}

		void Core::MakeCurrentContext()
		{
			glfwMakeContextCurrent(glfwWindow);
		}

		void Core::Close()
		{
			//glfwDestroyWindow(window);
			//glfwTerminate();
			//exit(EXIT_SUCCESS);
			glfwDestroyWindow(glfwWindow);
			glfwTerminate();
			exit(EXIT_SUCCESS);
		}

		void Core::Load()
		{
			started = true;
		}
	}
}