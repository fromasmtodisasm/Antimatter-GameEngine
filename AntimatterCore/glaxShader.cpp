#include "glaxShader.h"

namespace Antimatter {
	namespace glx
	{
		void Shader::SetUniform4f(int uniformID,float x,float y,float z,float w)
		{
			glProgramUniform4f(id, uniformID, x,y,z,w);
		}
		void Shader::SetUniform2f(int uniformID, float x, float y)
		{
			glProgramUniform2f(id, uniformID, x, y);
		}

		void Shader::SetUniform3f(int uniformID, float x, float y, float z)
		{
			glProgramUniform3f(id, uniformID, x, y, z);
		}

		void Shader::SetUniform4x4(int uniformID,const glm::mat4& mat)
		{
			glProgramUniformMatrix4fv(id, uniformID, 1, GL_FALSE, &mat[0][0]);
		}

		void Shader::SetUniform1i(int uniformID, int i)
		{
			glProgramUniform1i(id, uniformID,i);
		}

		void Shader::SetUniform1f(int uniformID, float f)
		{
			glProgramUniform1f(id, uniformID, f);
		}

		void Shader::SetUniformIntArray(int uniformID, int sizeoflist, int* list)
		{
			glProgramUniform1iv(id, uniformID, sizeoflist, list);
		}

		void Shader::DrawMesh(glx::VAO vbo)
		{
			glBindVertexArray(vbo.ID);
			int indexPos = vbo.indexPosition;
			int indexLength = vbo.indexLength;
			glDrawElements(GL_TRIANGLES, indexLength, GL_UNSIGNED_INT, (char*)NULL + (sizeof(unsigned int)*indexPos));
			glBindVertexArray(0);
		}

		void Shader::Compile()
		{
			id = glCreateProgram();
			cout << "\nShader Log: "<<name<<" \n Program ID: "<<id << endl;
			GLuint vid = CreateFromString(GL_VERTEX_SHADER, vertexString);
			cout << "Vertex shader..."<< endl;
			bool ready = true;
			ready = CheckShader(vid);
			if (geometryString != NULL)
			{
			
				GLuint gid = CreateFromString(GL_GEOMETRY_SHADER, geometryString);
				cout << "Geometry shader..." << endl;
				ready = CheckShader(gid);
			}

			GLuint fid = CreateFromString(GL_FRAGMENT_SHADER,fragmentString);
			cout << "Fragment shader..." << endl;
			ready = CheckShader(fid);
			if (ready)
				GetUniforms();
		}

		int Shader::GetUniformLocation(char* variableName)
		{
			return glGetUniformLocation(id, variableName);
		}

		void Shader::Debug()
		{
			cout << name << endl;
			Shader::DebugShaders(id);
			//ShaderLog(id, GL_VERTEX_SHADER);
			//ShaderLog(id, GL_GEOMETRY_SHADER);
			//ShaderLog(id, GL_FRAGMENT_SHADER);
		}

		void Shader::Select()
		{
			//call gluseprogram
			glx::Core::SelectShader(id);
			//	GetUniforms();
		}

		unsigned int Shader::CreateShaderFromFile(unsigned int shaderprogram, GLenum type, const char* filePath)
		{
			//DebugString(" \nglx::Shader ->> Creating Shader: program ");
			//cout << shaderprogram << " :type: " << type << endl;

			//creat vertex shader
			std::string shaderfile;
			LoadFile(filePath, shaderfile);
			const char* shaderString = shaderfile.c_str();
			int ShaderID = Create(type, shaderString);

			glAttachShader(shaderprogram, ShaderID);

			glLinkProgram(shaderprogram);

			//cout << "\n glx::Shader: Program ID: " << shaderprogram << endl;
			//DebugShaders(shaderprogram);

			return ShaderID;
		}

		GLuint Shader::CreateShaderProgram( const char* vertexshader, const char* geoshader, const char * fragshader)
		{

			//DebugString("glx::Shader->> Crafting Shader");
			id = glCreateProgram();

			Shader::CreateFromString( GL_VERTEX_SHADER, vertexshader);

			Shader::CreateFromString( GL_GEOMETRY_SHADER, geoshader);

			Shader::CreateFromString( GL_FRAGMENT_SHADER, fragshader);

			//Shader::DebugShaders(shaderprogram);
			return id;
		}

		GLuint Shader::CreateFromString(GLenum type, const char* fileString)
		{
			//creat vertex shader
			std::string shaderfile;
			const char* shaderString = fileString;
			unsigned int ShaderID = Create(type, shaderString);

			glAttachShader(id, ShaderID);

			glLinkProgram(id);
			//CheckShader();
			return ShaderID;
		}

		void Shader::DebugShaders(GLuint programID)
		{
			glUseProgram(programID);
			bool done = CompileSuccessful(programID);
			bool done2 = LinkSuccessful(programID);
			
			if (!CompileSuccessful(programID))
			{
				DebugString("Compile Unsuccessfull");
			}
			else {
				DebugString("Compile Successfull");
			}

			if (!LinkSuccessful(programID))
			{
				DebugString("Link Unsuccessfull");
			}
			else {
				DebugString("Link Successfull");
			}
			cout << "\n" << endl;
		}

		bool Shader::CompileSuccessful(int obj)
		{
			int status;
			glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
			return status == GL_TRUE;
		}

		bool Shader::LinkSuccessful(int obj)
		{
			int status;
			glGetProgramiv(obj, GL_LINK_STATUS, &status);
			return status == GL_TRUE;
		}

		GLuint Shader::Create(GLenum type, const char* src)
		{
			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &src, NULL);
			glCompileShader(shader);
			return shader;
		}

		bool Shader::CheckShader(GLuint shaderID)
		{
			GLint isCompiled = 0;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_TRUE)
			{
				cout << "...Shader ID: "<<shaderID<<" is Compiled\n" << endl;
				return true;
			}
			else {
				cout << " glx Shader::ShaderLog:  ERROR!: failed to compile" << endl;

				const GLubyte* version = glGetString(GL_VERSION);
				cout << "->> glx Shader::ShaderLog: OpenGL Version " << version << endl;
				const GLubyte* shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION_ARB);
				cout << "->> GLSL Version " << shaderVersion << endl;
				
				GLint logSize = 0;
				glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logSize);
				//cout << "->>>log Size: " << logSize << endl;
				if (logSize > 0)
				{
					std::vector<GLchar> errorLog(logSize);
					glGetShaderInfoLog(shaderID, logSize, &logSize, &errorLog[0]);
					cout << "->>> problem:" << endl;

					for (int i = 0; i < logSize; i++)
						cout << errorLog[i];
				}
				else {
					cout << "->>>> DOUBLE WAMMY!  Log size is 0!" << endl;
				}

				cout << "->>>> Shader Compilation FAILED!" << endl;
				//glDeleteShader(shaderID); // Don't leak the shader.

				return false;
			}
		}

		void Shader::LoadFile(const char* fn, string& str)
		{
			cout << "\nRenderer: file name: " << fn << "\n";
			ifstream stream;
			stream.open(fn);

			if (!stream.good())
			{
				cout << "-> failed to load file" << endl;
				return;
			}
			else
			{
				cout << "-> file is good" << endl;
			}

			char tmp[500];

			while (!stream.eof())
			{
				stream.getline(tmp, 500);
				str += tmp;
				str += "\n";
			}
		}
	}
}