#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "glaxDebug.h"
#include "glaxList.h"
#include "glaxTexture.h"
#include "glaxCore.h"
#include "glaxCamera.h"
#include <cstdlib>
#include "glaxRenderSettings.h"
#include "glaxShader.h"
#include "glaxFont.h"
#include "glaxVAO.h"
#include "glaxDefaultShader.h"
#include "glaxRender2D.h"

//projections
//https://stackoverflow.com/questions/46749675/opengl-mouse-coordinates-to-space-coordinates/46752492

//normal mapping
//http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/

//normals
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/

//attrib pointers
//http://stackoverflow.com/questions/18123733/glvertexattribpointer-change-vertex-attributes-layout

//manually computing normals
//http://www.codeguru.com/cpp/g-m/opengl/article.php/c2681/Computing-normals-to-achieve-flat-and-smooth-shading.htm
//http://gamedev.stackexchange.com/questions/8408/best-way-to-compute-vertex-normals-from-a-triangles-list

//actual code for calculating normals
//https://www.youtube.com/watch?v=f-vxvZGI8R0

//lighting
//http://www.tomdalling.com/blog/modern-opengl/06-diffuse-point-lighting/

//specular lighting and colour
//https://www.youtube.com/watch?v=nEt1OYtmOrE

//glsl quaternions
//http://www.geeks3d.com/20141201/how-to-rotate-a-vertex-by-a-quaternion-in-glsl/

//buffer streaming
//http://onrendering.blogspot.co.uk/2011/10/buffer-object-streaming-in-opengl.html

//https://www.opengl.org/wiki/Geometry_Shader_Examples

//object selection
//https://en.wikibooks.org/wiki/OpenGL_Programming/Object_selection

//billboards
//http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/

//textures and basic triangle
//https://learnopengl.com/#!Getting-started/Textures

//http://onrendering.blogspot.co.uk/2011/10/buffer-object-streaming-in-opengl.html

//persistent mapped buffers
//http://www.bfilipek.com/2015/01/persistent-mapped-buffers-in-opengl.html
//https://ferransole.wordpress.com/2014/06/08/persistent-mapped-buffers/
//https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glMapBufferRange.xhtml

//buffer streaming
//https://www.opengl.org/discussion_boards/showthread.php/170118-VBOs-strangely-slow/page4
//https://zachbethel.wordpress.com/2013/03/20/buffer-streamin-opengl/

//volume rendering
//https://stackoverflow.com/questions/9482572/volume-rendering-using-glsl-with-ray-casting-algorithm

namespace Antimatter
{
	namespace glx
	{

		class Render
		{
		private:
			static bool _transparencyEnabled;

		public:
			Render();

			int static const BufferSize = buffer_maximum;  //should be const?
			int static BufferPosition;
			int static IndexPosition;  //should be const?  or initialize in cpp file?
			
			static List<unsigned int> VertexBuffers;
			static List<unsigned int> IndexBuffers;
			static unsigned int vID;
			static unsigned int iID;
			static bool CreateBuffer;
			
			static bool InitializeDefaultRender;
			unsigned int static BufferIndex;
			bool static debug;
			int IndexLength = 0;
			glm::mat4 MVP;
			float fieldOfView;
			float tick;
			unsigned int bufferCount;
			bool debugScene;
			float ratio;
			float nearPlane;
			float farPlane;
			glx::VAO worldVbo;
			unsigned int aVBO;
			unsigned int aVAO;
			unsigned int aShader;
			int aProjectionID;
			GLint texAttrib;
			glm::vec3 LightPosition;
			glm::vec3 LightIntensity;
			glm::vec4 LightAttenuation;
			glm::vec3 ambientLight;
			glm::vec3 EyePosition;
			glm::vec4 BackGroundColour;
			glm::mat4 projectionMatrix;
			glm::mat4 inverseProjection;
			glm::mat4 viewMatrix;
			glm::mat4 worldMatrix;
			glm::mat4 canvasMatrix;
			glm::mat4 orthographicMatrix;
			glm::vec3 backcolour;
			string currentShader;
			GLFWwindow* window;
			glx::Core* gxCore;
			glx::Render2D render2d;
			glx::DefaultShader defaultShader;
			glx::DefaultShader lineShader;
			Camera DefaultCamera;
			static glx::VAO* DefaultSprite;
			static Render* Default;
			static void EnableTransparency(bool enable);
			static void EnableGammaCorrection(bool enable);
			static glm::vec4 ReadColour(double, double);
			static glx::VAO* BufferPrimitive(float data[], int datacount, unsigned int indices[], int indexcount);
			static int Render::ReadSelectionID(double x, double y);
			static glm::vec4 ReadPixel(double, double);
			static int currentBuffer;
			void InitializeShader();
			void SetPerspective(float,int, int,float,float);
			void ComputeView(const glm::vec3& Eye, const glm::vec3& Target, const glm::vec3& up);
			void DrawSprite(glx::VAO& vbo, const glm::mat4& transform, const glm::vec4& , int textureID, const RenderSetting& settings,bool billboard);
			void DrawMesh(glx::VAO&vao, const glm::mat4& transform, const glm::vec4& col, const RenderSetting& settings, int textureID);
			void DrawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& col, const glm::vec4& col2, float width,bool drawOnTop,bool orthographic);
			void DrawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& col, const glm::vec4& col2,float width, const glm::mat4& projectionMatrix,bool drawOnTop);
			glm::vec3 Render::CreateRay(float x, float y);
			void GLError();
			void GLErrorString();
			void OnUpdate();
			
			glm::vec2 Render::Get2dPoint(glm::vec3 point3D)
			{
				glm::vec4 p3d = MVP*glm::vec4(point3D,1);
				int winX = (int)round(((point3D.x + 1.f) / 2.0f) * (int)Core::windowWidth);	
				int winY = (int)round(((1.f - point3D.y) / 2.0f) *
					(int)Core::windowHeight);

				return glm::vec2(winX, winY);
			}

			glm::vec3 Render::Get3dPoint(glm::vec2 point2D) 
			{
				int wX;
				int wY;
				glfwGetWindowPos(glx::Core::glfwWindow, &wX, &wY);
				double x = 2.0 *Core::windowWidth / point2D.x - 1;
				double y = -2.0 * Core::windowHeight /point2D.y + 1;

				glm::vec3 point3D = glm::vec3(x, y, 0);
				glm::vec4 v = inverseProjection*glm::vec4(point3D,1);
				return glm::vec3(v);
			}

			static Render* GetRenderer()
			{
				if (Default == NULL)
				{
					Default = new Render();
					return Default;
				}
				else {
					return Default;
				}
			}

			void OnWindowResize(int width, int height)
			{
				//cout << "Renderer:: Resized Window: "<< width<< " : "<< height << endl;
				SetPerspective(fieldOfView, width,height, nearPlane, farPlane);
			}
			
			void SetLightPosition(float x, float y, float z)
			{
				LightPosition = glm::vec3(x, y, z);
			}

			void SetLightIntensity(float R, float G, float B)
			{
				glProgramUniform4f(defaultShader.id, defaultShader.LightIntensityID, R, G, B,1);
			}

			void SetAmbientLight(float R, float G, float B)
			{
				glProgramUniform4f(defaultShader.id, defaultShader.AmbientLightID, R,G,B,1);
			}

			void SetAttenuation(float x, float y,float z,float w)
			{
				LightAttenuation.x = x;
				LightAttenuation.y = y;
				LightAttenuation.z = z;
				LightAttenuation.w = w;
			}

			void SetBackGroundColour(float x, float y, float z, float a)
			{
				BackGroundColour = glm::vec4(x, y, z, a);
			}

			void ClearBufferBit()
			{
				glClearColor(BackGroundColour.x, BackGroundColour.y, BackGroundColour.z, BackGroundColour.a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

			void MoveCanvas(float x,float y)
			{
				canvasMatrix = glm::translate(canvasMatrix, glm::vec3(x, y, 0));
			}

			static void Debug()
			{
				cout <<"\n debugging renderer: "<<endl;
			}

			void ComputeDefaultCamera()
			{
				DefaultCamera.eye.GetWorldMatrix();
				ComputeView(DefaultCamera.eye.WorldPosition, DefaultCamera.target.WorldPosition,DefaultCamera.eye.GetUpDirection());
			}

			void Close()
			{
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glDeleteBuffers(1, &vID);
				glDeleteBuffers(1, &iID);
			}
			
			void DebugString(const char * text)
			{
				cout << "\n -> > renderer: " << text<<endl;
			}
		};	
	}
}


