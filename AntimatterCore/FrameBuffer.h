#pragma once
#include "glaxCore.h"
#include "tool.h"


namespace Antimatter {
	namespace glx {

		class FrameBuffer
		{
		public:
			FrameBuffer();
			~FrameBuffer();
			unsigned int Create();
			unsigned int CreateTexture();
			unsigned int id;
			unsigned int texture1;
			unsigned int texture2;
			GLenum RGBFormat = GL_RGB16F;
		};

		class MultisampleFramebuffer
		{
		public:
			MultisampleFramebuffer();
			~MultisampleFramebuffer();
			unsigned int CreateRenderTexture();
			unsigned int CreateDepthTexture();
			unsigned int Create();
			unsigned int id;
			unsigned int texture1;
			unsigned int texture2;
			unsigned int textureDepth;
			GLenum RGBFormat = GL_RGB16F;
		};
	}
}

