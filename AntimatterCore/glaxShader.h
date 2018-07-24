#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "glaxDebug.h"
#include "glaxVAO.h"

namespace Antimatter {
	namespace glx {
		
		public class Shader
		{
		public:

			Shader() {}

			const char* vertexString = R"(
			#version 400
				layout (location = 0) in vec3 inpos;
				//in vec4 inpos;
				uniform mat4 projection;
				void main(void)
				{
					gl_Position = projection*vec4(inpos,1);
					//gl_Position.w = 1;
				}
			)";

			const char* geometryString = NULL;

			const char* fragmentString =
				R"(
				#version 400


				void main(void) 
				{
				gl_FragColor = vec4(0,0.4f,0.2f,1);
				}
				)";

			unsigned int id;
			void SetUniform4f(int uniformID, float x, float y, float z, float w);
			void SetUniform3f(int uniformID, float x, float y, float z);
			void SetUniform4x4(int uniformID, const glm::mat4& mat);
			void SetUniform1i(int uniformID, int i);
			void SetUniform1f(int uniformID, float i);

			void SetUniformIntArray(int uniformID, int sizeoflist, int* list);
			char* name = "...Unnamed Shader...";
			//needs investigating
			//unsigned int _vsID;
			//unsigned int _gsID;
			//unsigned int _fsID;

			virtual void GetUniforms() {};

			static void DrawMesh(glx::VAO vbo);

			void Compile();

			int GetUniformLocation(char* variableName);

			void Debug();

			virtual void Select();

			~Shader()
			{
				//glDetachShader(id);
				/*
				glDetachShader(id, vsID);
				glDetachShader(id, fsID);
				glDeleteShader(vsID);
				glDeleteShader(fsID);
				if (gsID != 0)
				{
					glDetachShader(id, gsID);
					glDeleteShader(gsID);
				}
				*/
			}

			void Shader::SetUniform2f(int uniformID, float x, float y);

			static unsigned int CreateShaderFromFile(unsigned int shaderID, GLenum type, const char* filePath);

			GLuint CreateShaderProgram(const char* vertexshader, const char* geoshader, const char * fragshader);

			GLuint CreateFromString(GLenum type, const char* fileString);

			static void DebugShaders(GLuint programID);

			static bool CompileSuccessful(int obj);

			static bool LinkSuccessful(int obj);

			static GLuint Create(GLenum type, const char* src);

			bool CheckShader(GLuint shaderID);

			static void LoadFile(const char* fn, string& str);
		};
		
	}
}