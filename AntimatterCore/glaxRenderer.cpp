#include "glaxRenderer.h"

//ray tracing the mouse position
//http://stackoverflow.com/questions/8780871/color-picking-in-the-opengl

//colour picking byte conversion help
//http://gamedev.stackexchange.com/questions/75270/opengl-2-0-how-to-swap-buffers-for-color-picking

//bloom
//https://learnopengl.com/#!Advanced-Lighting/Bloom

//https://www.opengl.org/archives/resources/features/KilgardTechniques/oglpitfall/

//https://www.mvps.org/directx/articles/rayproj.htm

//shader colour id
//https://www.opengl.org/discussion_boards/showthread.php/198572-Want-to-be-taught-to-mouse-pick-with-raycasting-step-by-step-(C-FreeGLUT-OpenGL)

//screen to orld and vice versa
//http://webglfactory.blogspot.co.uk/2011/05/how-to-convert-world-to-screen.html

//defered rendering
//https://learnopengl.com/#!Advanced-Lighting/Deferred-Shading

namespace Antimatter {
	namespace glx{
		glx::VAO* glx::Render::DefaultSprite = NULL;  //initiaslising static member
		bool glx::Render::CreateBuffer = false;
		bool glx::Render::debug = false;
		int glx::Render::BufferPosition = 0;;
		int glx::Render::IndexPosition=0;  //should be const?  or initialize in cpp file?
		List<unsigned int> glx::Render::VertexBuffers;
		List<unsigned int> glx::Render::IndexBuffers;
		unsigned int glx::Render::vID = 0;;
		unsigned int glx::Render::iID = 0;
		unsigned int glx::Render::BufferIndex = 0;
		int glx::Render::currentBuffer = 0;
		bool glx::Render::InitializeDefaultRender = false;
		bool glx::Render::_transparencyEnabled=false;
		Render* glx::Render::Default = NULL;
		//http://stackoverflow.com/questions/10620408/initializing-a-static-pointer-in-c

		Render::Render()
		{
			gxCore = glx::Core::GetCore();
			
			__hook (&glx::Core::onWindowResized, gxCore, &glx::Render::OnWindowResize);
			__hook (&glx::Core::onFixedUpdate, gxCore, &glx::Render::OnUpdate);
			__hook (&glx::Core::onUpdate, gxCore, &glx::Render::OnUpdate);
			MVP = glm::mat4(1.f);
			canvasMatrix = glm::mat4(1.f);
			ambientLight = glm::vec3(0.f, 0.f, 0.05f);
			LightPosition = glm::vec3(0, 15, 10);
			LightIntensity = glm::vec3(1, 1, 1);
			LightAttenuation = glm::vec4(1, 0.01f, 0.005f, 5);
			BackGroundColour = glm::vec4(ambientLight.x, ambientLight.y, ambientLight.z, 1);
			projectionMatrix = glm::mat4(1.f);
			viewMatrix = glm::mat4(1.f);
			worldMatrix = glm::mat4(1.f);
			window = 0;
			InitializeDefaultRender = false;
			BufferIndex = 0;
			IndexPosition = 0;
			CreateBuffer = false;
			debug = false;
			EyePosition = glm::vec3(0.f, 0.f, 0.f);
			DefaultCamera = Camera();
			debugScene = false;
			fieldOfView = 45.0f;
			nearPlane = 0.001f;
			farPlane = 1000.f;
			render2d = glx::Render2D();
			InitializeShader();
			EnableTransparency(true);
			EnableGammaCorrection(true);
			SetPerspective(fieldOfView, glx::Core::windowWidth, glx::Core::windowHeight, nearPlane, farPlane);
			//probably need a pointer here
			orthographicMatrix = render2d.ortho;
		};

		void Render::OnUpdate()
		{

		}

		void Render::EnableTransparency(bool enable)
		{
			if (enable)
			{
				if (_transparencyEnabled)
					return;
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				_transparencyEnabled = true;
			}
			else {
				if (!_transparencyEnabled)
					return;
				glDisable(GL_BLEND);
				_transparencyEnabled = false;
			}
		}

		void Render::EnableGammaCorrection(bool enable)
		{
			if(enable)
				glEnable(GL_FRAMEBUFFER_SRGB);
			else
				glDisable(GL_FRAMEBUFFER_SRGB);
		}

		glx::VAO* Render::BufferPrimitive(float data[], int datacount, unsigned int indices[], int indexcount)
		{
			if (!InitializeDefaultRender)
			{
				GetRenderer();
				InitializeDefaultRender = true;
			}

			if (IndexPosition + indexcount > BufferSize ||
				BufferPosition + datacount > BufferSize)
			{
				BufferPosition = 0;
				IndexPosition = 0;
				BufferIndex++;
				CreateBuffer = false;
			}

			if (!CreateBuffer)
			{
				vID = 1;
				iID = 1;

				glGenBuffers(1, &vID);
				glGenBuffers(1, &iID);

				Core::SelectBuffer(vID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iID);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float)*BufferSize, NULL, GL_DYNAMIC_READ);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*BufferSize, NULL, GL_STATIC_READ);

				CreateBuffer = true;
				cout << "\n->>  create buffer: " << CreateBuffer<<endl;
				cout << "->>  vertex buffer: " << vID<<endl;
			}

			glx::VAO* vbo = new glx::VAO();
			glGenVertexArrays(1, &vbo->ID);
			glBindVertexArray(vbo->ID);

			if (debug)
			{
				cout << "\nSee VBO: " << vbo->ID;
				for (int i = 0; i < datacount; i++)
				{
					cout << "\n-> > renderer: Vertices " << i << " : " << data[i];
				}

				for (int i = 0; i < indexcount; i++)
				{
					cout << "\n-> > renderer: Indices " << i << " : " << indices[i];
				}

				cout << "\n-> > renderer: data length : " << datacount;
				cout << "\n-> > renderer: index length : " << indexcount<<endl;
			}

			glBindBuffer(GL_ARRAY_BUFFER, vID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iID);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*BufferPosition, sizeof(float)*datacount, data);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*IndexPosition, sizeof(unsigned int)*indexcount, indices);

			GLintptr vertexOffset = BufferPosition * sizeof(float);

			//vertex pointer
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*BufferPosition));

			//normal data
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*(BufferPosition + 3)));

			//UV data
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float)*(BufferPosition + 6)));

			BufferPosition += datacount;
			vbo->indexLength = indexcount;
			vbo->indexPosition = IndexPosition;
			IndexPosition += indexcount;
			vbo->bufferID = vID;
			if (debug)
				DebugObject<unsigned int>("Buffer Index Position", vbo->indexPosition);

			glBindVertexArray(0);
			vbo->buffer = BufferIndex;

			if (debug)
			{
				cout << "\n-> > Renderer: Buffered and added primitive";
				DebugObject<unsigned int>("New Data buffer Position", BufferPosition);
				DebugObject<unsigned int>("New Buffer Index Position", IndexPosition);
				vbo->debugMe("VAO");
			}

			return vbo;
		}

		glm::vec4 Render::ReadColour(double x, double y)
		{
			float pixel[4];
			GLsizei w = (GLsizei)glx::Core::windowWidth;
			GLsizei h = (GLsizei)glx::Core::windowHeight;
			glReadBuffer(GL_BACK);
			glReadPixels((GLint)x, (GLint)(h - y), (GLsizei)1, (GLsizei)1, GL_RGBA, GL_FLOAT, &pixel[0]);

			/*
			cout << "\nReading Pixel::\n"
			<< "X: " << x << " :Y: " << h-y << "\n"
			<< "R:"<<pixel[0]<<"\n"
			<< "G:" << pixel[1] << "\n"
			<< "B:" << pixel[2] << "\n"
			<< "A:" << pixel[3] << "\n";
			*/
			glm::vec4 col(pixel[0], pixel[1], pixel[2], pixel[3]);

			return col;
		}

		int Render::ReadSelectionID(double x, double y)
		{
			glFlush();
			glFinish();

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			int w = glx::Core::windowWidth;
			int h = glx::Core::windowHeight;
			//glReadBuffer(GL_COLOR_ATTACHMENT0);
			glReadBuffer(GL_BACK);
			//glDisable(GL_MULTISAMPLE);
			// Read the pixel at the center of the screen.
			// You can also use glfwGetMousePos().
			// Ultra-mega-over slow too, even for 1 pixel, 
			// because the framebuffer is on the GPU.
			Byte data[4];
			glReadPixels((GLuint)x, (GLuint)(h - y), (GLsizei)1, (GLsizei)1, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
			int pickedID =
				data[0] +
				data[1] * 256 +
				data[2] * 256 * 256;

			return pickedID;
		}

		glm::vec4 Render::ReadPixel(double x, double y)
		{
			Byte pixel[4];
			int w = glx::Core::windowWidth;
			int h = glx::Core::windowHeight;
			glReadBuffer(GL_BACK);
			//glFlush();
			glReadPixels((GLuint)x, (GLuint)(h-y), (GLsizei)1, (GLsizei)1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel[0]);

			/*
			cout << "\nReading Pixel::\n"
				<< "X: " << x << " :Y: " << h-y << "\n"
				<< "R:"<<pixel[0]<<"\n"
				<< "G:" << pixel[1] << "\n"
				<< "B:" << pixel[2] << "\n"
				<< "A:" << pixel[3] << "\n";
				*/
			glm::vec4 col((float)pixel[0]/255,(float)pixel[1]/255,(float)pixel[2]/255,(float)pixel[3]/255);

			return col;
		}
	
		//http://gamedev.stackexchange.com/questions/51820/how-can-i-convert-screen-coordinatess-to-world-coordinates-in-opentk
		glm::vec3 Render::CreateRay(float x, float y)
		{
		//	cout << "Client X: " << posx << " Y: " << posy << endl;
		/*
			float screenWidth = glx::Core::windowWidth;
			float screenHeight = glx::Core::windowHeight;

			glm::vec4 lRayStart_NDC(
				(x / screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
				(y / screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
				-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
				1.0f
			);
			glm::vec4 lRayEnd_NDC(
				(x/ screenWidth - 0.5f) * 2.0f,
				(x / screenHeight - 0.5f) * 2.0f,
				0.0,
				1.0f
			);

			glm::mat4 M = glm::inverse(projectionMatrix* viewMatrix);
			glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
			glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;
			
			glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
			lRayDir_world = glm::normalize(lRayDir_world);
			return lRayDir_world;
			//clip space coords
			*/
			//float vr = tanf(fieldOfView*0.5f);
			
			float posx = (2.f * x) / glx::Core::windowWidth - 1.f;// -1.0f) / gxWindow->ratio;
			float posy = 1.0f - 2.f * y / glx::Core::windowHeight;
			float posz = 0.f;

			glm::vec3 ray_nds = glm::vec3(posx, posy, posz);
			glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);


			glm::vec4 ray_eye = inverseProjection * ray_clip;
			ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
			glm::vec4 v4 = glm::vec4(glm::inverse(viewMatrix) * ray_eye);
			glm::vec3 ray_wor = glm::vec3(v4.x, v4.y, v4.z);
			glm::vec3 dir = glm::normalize(ray_wor);
			
			return dir;
		}

		void Render::SetPerspective(float fov,int width,int height,float nearView, float farView)
		{
			if (height == 0)
				height = 1;

			ratio = (float)width / (float)height;
			glViewport(0, 0, width, height);
			nearPlane = nearView;
			farPlane = farView;
			projectionMatrix = glm::perspective(glm::radians(fieldOfView), ratio, nearView, farView);
			inverseProjection = glm::inverse(projectionMatrix);
		}

		void Render::ComputeView(const glm::vec3& Eye, const glm::vec3& Target,const glm::vec3& up)
		{
			viewMatrix = glm::lookAt(Eye, Target, glm::vec3(up.x,up.y,up.z));
			MVP = projectionMatrix*viewMatrix;
			EyePosition = Eye;
		}

		void Render::DrawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& col, const glm::vec4& col2, float width, bool drawOntop,bool orthograhic)
		{
			
			if (orthograhic)
				DrawLine(point1, point2, col, col2,width, render2d.ortho, drawOntop);
			else
				DrawLine(point1, point2, col, col2,width, MVP, drawOntop);

		}
		void Render::DrawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& col, const glm::vec4& col2,float width, const glm::mat4& projectionMatrix, bool drawOnTop)
		{
			if (col.w < 1.f)
			{
				EnableTransparency(true);
				glDisable(GL_DEPTH_TEST);
			}
			if (drawOnTop)
				glClear(GL_DEPTH_BUFFER_BIT);

			lineShader.Select();
			lineShader.SetUniform4f( lineShader.LinePointA, point1.x, point1.y,point1.z, 1);
			lineShader.SetUniform4f(lineShader.LinePointB, point2.x, point2.y, point2.z, 1);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_TRIANGLES);
			
			glm::mat4 worldtrans = glm::mat4(1.f);
			glm::mat4 trans = projectionMatrix*worldtrans;
			lineShader.SetUniform4x4(lineShader.TransformID, trans);

			lineShader.SetUniform4f(lineShader.LineColour1, col.x, col.y, col.z, col.w);
			lineShader.SetUniform4f(lineShader.LineColour2, col2.x, col2.y, col2.z, col2.w);

			int setting[4];
			setting[0] = 0;
			setting[1] = 0;
			setting[2] = 0;
			setting[3] = 0;

			lineShader.SetUniformIntArray(lineShader.settingsID, 4, setting);

			worldVbo.DrawMesh();

			if (col.w < 1)
			{
				glEnable(GL_DEPTH_TEST);
			}
		}
		
		void Render::DrawMesh(glx::VAO& vbo, const glm::mat4& transform, const glm::vec4& col, const RenderSetting& settings, int textureID)
		{
			if (settings.onTop)
				glClear(GL_DEPTH_BUFFER_BIT);

			defaultShader.Select();

			if (col.w < 1)
			{
				//EnableTransparency();
			}
			if (settings.solid)
			{
				glPolygonMode(GL_FRONT, GL_FILL);
				glEnable(GL_TRIANGLES);
			}
			if (settings.wireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth(0.8f);
			}

			if (settings.points)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
				glEnable(GL_POINTS);
			}

			glm::mat4 trans = MVP*transform;
			defaultShader.SetUniform4x4(defaultShader.TransformID, trans);

			defaultShader.SetUniform4f(defaultShader.ColourID, col.x, col.y, col.z, col.w);

			glm::mat4 scaledmatrix = glm::transpose(glm::inverse(transform));
			defaultShader.SetUniform4x4(defaultShader.ScaledMatrixID, scaledmatrix);

			defaultShader.SetUniform4x4(defaultShader.PositionMatrixID, transform);

			defaultShader.SetUniform3f(defaultShader.EyePositionID, EyePosition.x, EyePosition.y, EyePosition.z);

			defaultShader.SetUniform3f(defaultShader.LightPositionID, LightPosition.x, LightPosition.y, LightPosition.z);

			defaultShader.SetUniform4f(defaultShader.LightAttenID, LightAttenuation.x, LightAttenuation.y, LightAttenuation.z, LightAttenuation.w);

			int setting[4];
			setting[0] = (int)textureID;
			setting[1] = (int)settings.screenSpace;
			setting[2] = 0;
			setting[3] = (int)settings.lightingEnabled;

			defaultShader.SetUniformIntArray(defaultShader.settingsID, 4, setting);

			if (textureID != 0)
			{
				glBindTexture(GL_TEXTURE_2D, textureID);
				glProgramUniform1i(defaultShader.id, defaultShader.TextureID, 0);
			}

			vbo.DrawMesh();

			if (col.w < 1)
			{
				//glEnable(GL_DEPTH_TEST);
			}

			if (settings.disableCulling)
			{
				//	glEnable(GL_CULL_FACE);
				//glCullFace(GL_BACK);
			}
		}

		void Render::DrawSprite(glx::VAO& vbo, const glm::mat4& transform, const glm::vec4& col, int textureID, const RenderSetting& settings,bool billboard)
		{

			if (col.w < 1)
			{
				EnableTransparency(true);
				glDisable(GL_DEPTH_TEST);
			}

			defaultShader.Select();
			glm::mat4 projection;
			if (settings.screenSpace)
				projection = orthographicMatrix;
			else
				projection = projectionMatrix;

			glm::mat4 trans;
			glm::mat4 scale = glm::transpose(trans);
			
			if (settings.screenSpace)
				trans = canvasMatrix*transform;
			else if (billboard)
			{
				glm::mat4 modelView = viewMatrix*transform;
				//modelView[0][0] = trans[0][0];
				modelView[0][1] = 0.0f;
				modelView[0][2] = 0.0f;

				// Second colunm.
				modelView[1][0] = 0.0f;
				//modelView[1][1] = trans[1][1];
				modelView[1][2] = 0.0f;

				// Thrid colunm.
				modelView[2][0] = 0.0f;
				modelView[2][1] = 0.0f;
				//modelView[2][2] = trans[2][2];
				trans = projection*modelView;
			}else
				trans = MVP*transform;

			int setting[4];
			setting[0] = (int)textureID;
			setting[2] = (int)settings._selectable;
			setting[1] = (int)settings.screenSpace;
			setting[3] = (int)settings.lightingEnabled;

			glm::mat4 scaledmatrix = glm::transpose(glm::inverse(trans));
			defaultShader.SetUniform4x4(defaultShader.ScaledMatrixID, scaledmatrix);
			
			defaultShader.SetUniformIntArray(defaultShader.settingsID, 4.f, setting);
			defaultShader.SetUniform4x4(defaultShader.TransformID, trans);
			defaultShader.SetUniform4x4(defaultShader.viewID,  viewMatrix);
			defaultShader.SetUniform4x4(defaultShader.projectionID,  projection);

			glm::mat4 canvasmatrix(1.0f);
			canvasmatrix = glm::translate(canvasmatrix, glm::vec3(0.f, 0.f, 0.f));
			defaultShader.SetUniform2f(defaultShader.screenCoordsID, glx::Core::windowWidth, glx::Core::windowHeight);
			defaultShader.SetUniform4x4(defaultShader.canvasMatrixID,  canvasmatrix);
			defaultShader.SetUniform4f(defaultShader.ColourID, col.x, col.y, col.z, col.w);

			defaultShader.SetUniform4x4(defaultShader.PositionMatrixID,  trans);
			GLintptr indexPointer = vbo.indexPosition * sizeof(unsigned int);

			defaultShader.SetUniform3f(defaultShader.EyePositionID, EyePosition.x, EyePosition.y, EyePosition.z);

			defaultShader.SetUniform4f(defaultShader.LightAttenID, LightAttenuation.x, LightAttenuation.y, LightAttenuation.z, LightAttenuation.w);

			if (textureID != 0)
			{
				glBindTexture(GL_TEXTURE_2D, textureID);
				glProgramUniform1i(defaultShader.id,defaultShader.TextureID, 0);
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_TRIANGLES);

			if(settings.disableCulling)
			{
				glDisable(GL_CULL_FACE);
			}

			vbo.DrawMesh();

			if (settings.disableCulling)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			if (col.w < 1)
			{
				EnableTransparency(false);
				glEnable(GL_DEPTH_TEST);
			}
			if (textureID != 0)
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		void Render::GLError()
		{
			cout << "->> Renderer: OpenGL Error: " << glGetError() << endl;
		}

		void Render::GLErrorString()
		{
			cout << "->> Renderer: OpenGL Error String: " << glewGetErrorString(glGetError()) << endl;
		}
	}
}


