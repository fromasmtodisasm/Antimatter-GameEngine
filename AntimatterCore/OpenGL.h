#pragma once
//https://www.codeproject.com/Articles/16051/Creating-an-OpenGL-view-on-a-Windows-Form
//http://zolver.blogspot.co.uk/2012/06/opengl-rendering-control-under-managed.html
#include <windows.h>
#include "glaxCore.h"
#include <GL/GL.h>	


using namespace System::Windows::Forms;

namespace Antimatter
{
	namespace Games {
		public ref class OpenGL : public System::Windows::Forms::NativeWindow
		{
		public:
			OpenGL(System::Windows::Forms::Form ^ parentForm)
			{
				CreateOpenGLContext(parentForm);
			}
			
			int width;
			int height;
			void SetViewPort()
			{
				glx::Core::ResizeWindow(width, height);
				if (this)SetWindowPos((HWND)this->Handle.ToPointer(), NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			}
			void BindRenderContext()
			{
				if (!(RenderContext = wglGetCurrentContext()))
				{
					MessageBox::Show("GUI GL Render context failed");
					if ((RenderContext = wglCreateContext(m_hDC)) == NULL)
					{
						MessageBox::Show("wglCreateContext Failed");
						return;
					}
					else {
						MessageBox::Show("GUI gl created new context");
						SwapBuffers(m_hDC);
						SetViewPort();
					}
					return;
				}

				if ((wglMakeCurrent(m_hDC, RenderContext)) == NULL)
				{
					MessageBox::Show("wglMakeCurrent Failed");
					return;
				}
			}

			void CreateOpenGLContext(Form^ parentForm)
			{
				CreateParams^ cp = gcnew CreateParams;
				// Set the position on the form
				cp->X = 0;
				cp->Y = 0;
				cp->Height = parentForm->Height;
				cp->Width = parentForm->Width;
				width = parentForm->Width;
				height = parentForm->Height;
				// Specify the form as the parent.
				cp->Parent = parentForm->Handle;

				// Create as a child of the specified parent and make OpenGL compliant (no clipping)
				cp->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
				
				// Create the actual window
				this->CreateHandle(cp);
				
				m_hDC = GetDC((HWND)this->Handle.ToPointer());
				
				if (m_hDC)
					MySetPixelFormat(m_hDC);
				//initialize GLEW
				glx::Core::DefaultCore->InitializeGLEW();
			}

			GLint MySetPixelFormat(HDC hdc)
			{
				PIXELFORMATDESCRIPTOR pfd = {
					sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd 
					1,                                // version number 
					PFD_DRAW_TO_WINDOW |              // support window 
					PFD_SUPPORT_OPENGL |              // support OpenGL 
					PFD_DOUBLEBUFFER,                 // double buffered 
					PFD_TYPE_RGBA,                    // RGBA type 
					24,                               // 24-bit color depth 
					0, 0, 0, 0, 0, 0,                 // color bits ignored 
					0,                                // no alpha buffer 
					0,                                // shift bit ignored 
					0,                                // no accumulation buffer 
					0, 0, 0, 0,                       // accum bits ignored 
					32,                               // 32-bit z-buffer     
					0,                                // no stencil buffer 
					0,                                // no auxiliary buffer 
					PFD_MAIN_PLANE,                   // main layer 
					0,                                // reserved 
					0, 0, 0                           // layer masks ignored 
				};

				GLint  iPixelFormat;

				// get the device context's best, available pixel format match 
				if ((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
				{
					MessageBox::Show("ChoosePixelFormat Failed");
					return 0;
				}

				// make that match the device context's current pixel format 
				if (SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
				{
					MessageBox::Show("SetPixelFormat Failed");
					return 0;
				}

				if ((m_hglrc = wglCreateContext(m_hDC)) == NULL)
				{
					MessageBox::Show("wglCreateContext Failed");
					return 0;
				}

				if ((wglMakeCurrent(m_hDC, m_hglrc)) == NULL)
				{
					MessageBox::Show("wglMakeCurrent Failed");
					return 0;
				}

				return 1;
			}

			void MakeCurrent()
			{
				if (wglMakeCurrent(m_hDC, RenderContext) == NULL)
				{
					MessageBox::Show("Failed to set opengl device");
				}
			}

			System::Void Render(System::Void)
			{
				//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear screen and depth buffer
			}

			void ClearColour(float r, float g, float b, float a)
			{
				// Clear the color and depth buffers.
				glClearColor(r, g, b, a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			System::Void SwapBuffer(System::Void)
			{
				SwapBuffers(m_hDC);
			}

		private:
			HDC m_hDC;
			HGLRC m_hglrc;
			HGLRC RenderContext;

			GLfloat	rtri;				// Angle for the triangle
			GLfloat	rquad;				// Angle for the quad

		protected:
			~OpenGL(System::Void)
			{
				this->DestroyHandle();
			}


			void SetDeviceContext()
			{
				/*
				HDC oldDC = wglGetCurrentDC();
				GLint  iPixelFormat = GetPixelFormat(oldDC); //

				PIXELFORMATDESCRIPTOR pfd = {
					sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd 
					1,                                // version number 
					PFD_DRAW_TO_WINDOW |              // support window 
					PFD_SUPPORT_OPENGL |              // support OpenGL 
					PFD_DOUBLEBUFFER,                 // double buffered 
					PFD_TYPE_RGBA,                    // RGBA type 
					24,                               // 24-bit color depth 
					0, 0, 0, 0, 0, 0,                 // color bits ignored 
					0,                                // no alpha buffer 
					0,                                // shift bit ignored 
					0,                                // no accumulation buffer 
					0, 0, 0, 0,                       // accum bits ignored 
					32,                               // 32-bit z-buffer     
					0,                                // no stencil buffer 
					0,                                // no auxiliary buffer 
					PFD_MAIN_PLANE,                   // main layer 
					0,                                // reserved 
					0, 0, 0                           // layer masks ignored 
				};


				if (iPixelFormat == 0)
				{
					// get the device context's best, available pixel format match 
					if ((iPixelFormat = ChoosePixelFormat(DeviceHandle, &pfd)) == 0)
					{
						MessageBox::Show("ChoosePixelFormat Failed");
						return;
					}
				}

				// make that match the device context's current pixel format 
				if (SetPixelFormat(DeviceHandle, iPixelFormat, &pfd) == FALSE)
				{
					MessageBox::Show("SetPixelFormat Failed");
					return;
				}
				*/

				/*
				if ((RenderContext = wglCreateContext(DeviceHandle)) == NULL)
				{
					MessageBox::Show("wglCreateContext Failed");
					return 0;
				}

				if ((wglMakeCurrent(DeviceHandle, RenderContext)) == NULL)
				{
					MessageBox::Show("wglMakeCurrent Failed");
					return 0;
				}


				return 1;
				*/
			}
			
		};
	}
}