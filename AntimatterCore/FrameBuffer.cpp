#include "FrameBuffer.h"


using namespace Antimatter::glx;
FrameBuffer::FrameBuffer()
{
}


FrameBuffer::~FrameBuffer()
{
}

unsigned int Antimatter::glx::FrameBuffer::CreateTexture()
{
	int i = glGetError();
	if (i != 0)
	{
		//Antimatter::Games::tool::show(i);
	}
	unsigned int textureID;
	glGenTextures(1, &textureID);
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, RGBFormat, W, H, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	i = glGetError();
	if (i != 0)
	{
		Antimatter::Games::tool::show(i);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return textureID;
}

unsigned int FrameBuffer::Create()
{
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;
	glEnable(GL_TEXTURE_2D);
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	texture1 = CreateTexture();
	texture2 = CreateTexture();
	//fbo_depth_texture = CreateRenderTexture(true);
	//glGenRenderbuffers(1, &texture1);
	//glGenRenderbuffers(1, &texture2);
	//glBindRenderbuffer(GL_RENDERBUFFER, fbo_render_texture);
	//glBindRenderbuffer(GL_RENDERBUFFER, 4, GL_RGB10_A2, W, H);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture1, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture2, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo_depth_texture, 0);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);
	int i = glCheckFramebufferStatus(id);
	if (i != 0)
	{
		Antimatter::Games::tool::show("Frame buffer problem", i);
		return 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return id;
}

Antimatter::glx::MultisampleFramebuffer::MultisampleFramebuffer()
{
}

Antimatter::glx::MultisampleFramebuffer::~MultisampleFramebuffer()
{
}

unsigned int Antimatter::glx::MultisampleFramebuffer::CreateRenderTexture()
{
	int i = glGetError();
	if (i != 0)
	{
		//Antimatter::Games::tool::show(i);
	}
	unsigned int textureID;
	glGenTextures(1, &textureID);
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, glx::Core::smoothness, GL_RGBA, W, H, GL_TRUE);
	
	i = glGetError();
	if (i != 0)
	{
		Antimatter::Games::tool::show(i);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return textureID;
	
}

unsigned int Antimatter::glx::MultisampleFramebuffer::CreateDepthTexture()
{
	int i = glGetError();
	if (i != 0)
	{
		//Antimatter::Games::tool::show(i);
	}
	unsigned int textureID;
	glGenTextures(1, &textureID);
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, W, H, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	i = glGetError();
	if (i != 0)
	{
		Antimatter::Games::tool::show(i);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

unsigned int Antimatter::glx::MultisampleFramebuffer::Create()
{
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;
	glEnable(GL_TEXTURE_2D);

	texture1 = CreateRenderTexture();
	texture2 = CreateRenderTexture();  //glow buffer uses and float for pixel format so we can store values > 1 in them
	textureDepth = CreateRenderTexture();

	GLint maxSamples;
	glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
	maxSamples = glx::Core::smoothness;
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	//Antimatter::Games::tool::show(maxSamples);
	glGenRenderbuffers(1, &texture1);
	glBindRenderbuffer(GL_RENDERBUFFER, texture1);
	//glRenderbufferStorageMultisample(GL_RENDERBUFFER, maxSamples, GL_RGB10_A2, W, H);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, maxSamples, RGBFormat, W, H);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, texture1);

	//may not need to be multisampled
	glGenRenderbuffers(1, &texture2);
	glBindRenderbuffer(GL_RENDERBUFFER, texture2);
	//glRenderbufferStorageMultisample(GL_RENDERBUFFER, maxSamples, GL_RGB10_A2, W, H);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, maxSamples, RGBFormat, W, H);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, texture2);


	//glGenRenderbuffers(1, &textureDepth);
	//glBindRenderbuffer(GL_RENDERBUFFER, textureDepth);
	//glRenderbufferStorageMultisample(GL_RENDERBUFFER, maxSamples, GL_DEPTH_COMPONENT, W, H);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, textureDepth);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, textureDepth);

	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);
	int i = glCheckFramebufferStatus(id);
	if (i != 0)
	{
		Antimatter::Games::tool::show("Frame buffer problem", i);
		return 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//to easily fetch the framebuffer textures in shaders, we should combine the
	//multisample fbo into on texture and since we cannot use the back buffer,
	//we'll create a normal fbo instead.  Perhaps we can blit straight to a texture instead
	//CreateFBO();
	return id;
}
