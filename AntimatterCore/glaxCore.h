#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Core.h"
#include "glaxWindow.h"
#include "callIntrinsics.h"


//#include "glaxFont.h"

//https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Text_Rendering_01

namespace Antimatter
{
	namespace glx
	{
		[event_source(native)]
		class Core
		{

			static void window_size_callback(GLFWwindow* window, GLsizei width, GLsizei height)
			{

				if (height == 0)										// Prevent A Divide By Zero By
				{
					height = 1;										// Making Height Equal One
				}

				Core* ep = (Core*)glfwGetWindowUserPointer(window);
				glx::Core::windowHeight = (int)height;
				glx::Core::windowWidth = (int)width;
				glx::Core::ratio = (float)width / (float)height;
				__raise ep->onWindowResized(width, height);
				Antimatter::Games::Core::Resize_CallBack(width, height);
			}

			//when the window has lost/gained focus
			static void window_focus_callback(GLFWwindow* window, int focused)
			{
				if (!focused)
					glfwWaitEvents();
			}

			//file drop callback
			static void drop_callback(GLFWwindow* window, int count, const char** paths)
			{
				Antimatter::Games::Core::Drop_CallBack(count, paths);
			}

			//key board callback
			static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
					glfwSetWindowShouldClose(window, GL_TRUE);
				Antimatter::Games::Core::Keyboard_RaiseEvents(key, action, scancode, mods);
			}

			static void error_callback(int error, const char* description)
			{
				Antimatter::Games::Core::Error_CallBack(description);
			}

			//mouse button callback
			static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
			{
				Antimatter::Games::Core::MouseButton_CallBack(button, action, mods);
			}

			// mouse position callback
			static void position_callback(GLFWwindow* window, double x, double y)
			{
				Antimatter::Games::Core::MousePosition_CallBack(x, y);
			}

			// records how far the y axis has been scrolled
			static void OnScroll(GLFWwindow* window, double deltaX, double deltaY)
			{
				Antimatter::Games::Core::MouseScroll_CallBack(deltaX, deltaY);
			}

		public:
			Core();
			static void SetCurrentContext(GLFWwindow* GLFWWindowPointer);
			const char* OutputTitle;
			const char ** dropFiles;
			void Initialize();
			void InitializeGLEW();
			void HideWindow();
			void MakeCurrentContext();
			bool started = false;
			virtual void Load();
			char* Title;
			std::string exePath;
			static bool Initialized;
			float RotationY;
			float Frame;
			static GLFWwindow *glfwWindow;
			static float ratio;
			static int currentBuffer;
			static int currentShader;
			static bool trackOpenGLStates;
			static glx::Core* DefaultCore;
			__event void onWindowResized(int, int);
			__event void onUpdate();
			__event void onFixedUpdate();
			static bool SelectBuffer(int);
			static bool SelectShader(int);
			float fps;
			float fixedFps;
			float deltaTime;
			//gcroot<Glaxion::Games::Core^> delegates;
			glm::vec4* BackColour;
			float MouseX;
			float MouseY;
			void Close();
			float alpha = 1.f;
			static int windowHeight;
			static int windowWidth;
			static int smoothness;

			static void ResizeWindow(int width, int height)
			{
				if (height == 0)										// Prevent A Divide By Zero By
				{
					height = 1;										// Making Height Equal One
				}

				glx::Core::windowHeight = (int)height;
				glx::Core::windowWidth = (int)width;
				__raise Core::DefaultCore->onWindowResized(width, height);
				Antimatter::Games::Core::Resize_CallBack(width, height);
				//glViewport(0, 0, width, height);
			}

			static void debugGlm(glm::vec4 v,const char* text)
			{
				cout << "\n Debugging glm: " << text << endl;
				cout << "V.x: " << v.x << " V.y: " << v.y << " V.z: " << v.z << " V.w: " << v.w << endl;
			}

			static void debugGlm(glm::vec3 v, const char* text)
			{
				cout << "\n Debugging glm: " << text << endl;
				cout << "V.x: " << v.x << " V.y: " << v.y << " V.z: " << v.z << endl;
			}

			static void debugGlm(glm::vec2 v, const char* text)
			{
				cout << "\n Debugging glm: " << text << endl;
				cout << "V.x: " << v.x << " V.y: " << v.y << endl;
			}
			
			void SetWindowTitle(const char* Text)
			{
				glfwSetWindowTitle(glfwWindow, Text);
			}

			static Core* GetCore()
			{
				if (DefaultCore == NULL)
				{
					DefaultCore = new Core();
					return DefaultCore;
				}
				else {
					return DefaultCore;
				}
			}

			static void PrintGLFWVersion()
			{
				Console::ForegroundColor = ConsoleColor::Green;
				int major;
				int minor;
				int version;
				glfwGetVersion(&version, &major, &minor);
				cout << "\nGLFW::Version: " << version << "." << major << "." << minor << endl;
				Console::ForegroundColor = ConsoleColor::White;
			}

			static unsigned int GenerateWorldBuffer()
			{
				unsigned int ID;
				glGenBuffers(1, &ID);
				glBindBuffer(GL_ARRAY_BUFFER, ID);

				float vertices[] =
				{
					0.f
				};
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_READ);
				glBindBuffer(GL_ARRAY_BUFFER, currentBuffer);  //might as well restore the last buffer instead of reseting it
				return ID;
			}

			~Core();
			/*
			void setIcon(long window, BufferedImage img)
			{
				GLFWimage image = GLFWimage.malloc();
				image.set(img.getWidth(), img.getHeight(), loadImageToByteBuffer(img));

				GLFWimage.Buffer images = GLFWimage.malloc(1);
				images.put(0, image);
				
				glfwSetWindowIcon(window, images);

				images.free();
				image.free();
			}
			*/
		private:

		};
	}
}
