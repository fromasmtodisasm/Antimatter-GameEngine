#pragma once
#include "SystemHeader.h"
#include "glaxTexture.h"
#include "glaxCore.h"
#include "glaxShader.h"
#include "ShaderFor2DPrimitives.h"
#include "ShaderFor2DGlow.h"
#include "tool.h"
#include "FrameBuffer.h"
#include "ShaderForDiamonds2D.h"

//drawing 2d sprites
//https://gamedev.stackexchange.com/questions/25411/basics-of-drawing-in-2d-with-opengl-3-shaders

namespace Antimatter {
	namespace glx {

		class Render2D
		{
		public:
			Render2D()
			{
				lightPosition = glm::vec3(0.f);
				RenderTexturePrecision = GL_RGB16F;
				//RenderTexturePrecision = GL_RGB10_A2;
			};

			unsigned int worldbuffer_id;
			unsigned int aVAO;
			glm::mat4 projection;
			glm::vec3 lightPosition;
			glm::mat4 ortho;
			ShaderForPrimitives2D triangleShader;
			ShaderForPrimitives2D lineShader;
			ShaderForDiamonds2D diamondShader;
			ShaderFor2DGlow glowShader;
			GLenum RenderTexturePrecision;
			void EnableDualBufferDraw();
			void EnableSingleBufferDraw();
			glm::vec4 ReadPixel(float x, float y);
			void InvokeDraw(GLsizei);
			void DrawBuffers();
			void StartMultiBufferDraw();
			FrameBuffer framebuffer1;
			FrameBuffer framebuffer2;
			FrameBuffer framebufferRead;	
			MultisampleFramebuffer framebufferMS;
			GLuint Start()
			{
				SetupShader();
				SetupBuffer();
				ortho = GetOrthographicMatrix();
				return 1;
			}
			
			int Render2D::SetupShader()
			{
				cout << "\n--------> glax render 2D:: creating 2D shader" << endl;
				triangleShader = ShaderForPrimitives2D();
				triangleShader.name = "Triangle Shader";
				triangleShader.Compile();

				lineShader = ShaderForPrimitives2D();
				lineShader.name = "Line Shader";
				lineShader.geometryString = lineShader.lineGeometryShader;
				lineShader.Compile();

				glowShader = ShaderFor2DGlow();
				glowShader.name = "Glow Shader";
				glowShader.Compile();


				diamondShader = ShaderForDiamonds2D();
				diamondShader.name = "Diamond Shader";
				diamondShader.Compile();
				return triangleShader.id;
			}

			void  Render2D::SetupBuffer()
			{
				worldbuffer_id = glx::Core::GenerateWorldBuffer();

				framebuffer1 = FrameBuffer();
				framebuffer1.Create();

				framebuffer2 = FrameBuffer();
				framebuffer2.Create();

				framebufferRead = FrameBuffer();
				framebufferRead.Create();

				framebufferMS = MultisampleFramebuffer();
				framebufferMS.Create();
			}

			glm::mat4 GetOrthographicMatrix();

			void Render2D::DrawTriangle(const glm::vec2& A, const glm::vec4& colourA, const glm::vec2& B, const glm::vec4& colourB, const glm::vec2& C, const glm::vec4& colourC, int enableLighting);

			void Render2D::DrawDiamond(float x, float y, const glm::vec4& colour, const glm::vec4& glow, float scale);

			void Render2D::DrawLine(const glm::vec2& A, const glm::vec4& colourA, const glm::vec2& B, const glm::vec4& colourB, float thickness, int enableLighting)
			{
				lineShader.Select();

				//temp hack for transparency.  
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				//probably only need to draw on one side
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_TRIANGLES);
				ortho = GetOrthographicMatrix();
				lineShader.SetUniform1i(lineShader.enableLightingID, enableLighting);
				lineShader.SetUniform1f(lineShader.thicknessID, thickness);
				lineShader.SetUniform4x4(lineShader.TransformID, ortho);
				float Z = 0.0f;
				glm::mat4 matrixA = glm::mat4(0.f);
				matrixA[0] = glm::vec4(A, Z,1.f);
				matrixA[1] = colourA;
				if (colourA.w > 1.f)
					matrixA[2] = colourA;
				lineShader.SetUniform4x4(lineShader.vertexMatrix1ID, matrixA);

				glm::mat4 matrixB = glm::mat4(0.f);
				matrixB[0] = glm::vec4(B,Z, 1.f);
				matrixB[1] = colourB;
				if (colourB.w > 1.f)
					matrixB[2] = colourB;
				lineShader.SetUniform4x4(lineShader.vertexMatrix2ID, matrixB);

				lineShader.SetUniform3f(lineShader.LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z);

				InvokeDraw(6);
			}
		};
	}
}

