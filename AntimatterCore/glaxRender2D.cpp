#include "glaxRender2D.h"
#include "tool.h"

void Antimatter::glx::Render2D::InvokeDraw(GLsizei indexCount)
{
	Core::SelectBuffer(worldbuffer_id);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		1,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, indexCount);
	glDisableVertexAttribArray(0);
}

//http://ake.in.th/2013/04/02/offscreening-and-multisampling-with-opengl/
//http://litherum.blogspot.co.uk/2014/01/how-multisampling-works-in-opengl.html

void Antimatter::glx::Render2D::EnableDualBufferDraw()
{
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::mat4 Antimatter::glx::Render2D::GetOrthographicMatrix()
{
	float W = (float)glx::Core::windowWidth;
	float H = (float)glx::Core::windowHeight;
	/*
	float pixelsToUnit = 1.f;
	float scale = H / HEIGHT;
	pixelsToUnit *= scale;
	float orthographicSize = (H / 2.0f) / pixelsToUnit;
	*/
	float ratio = ((float)Core::windowWidth / (float)Core::windowHeight);
	float world_scale = ratio*240.f;
	glm::vec3 camera_scale = glm::vec3(world_scale);
	glm::mat4 world = glm::scale(glm::mat4(1.f), camera_scale);
	glm::mat4 view = glm::translate(world, glm::vec3(0.f, 0.f, 0.f));
	//glm::mat4 ortho = glm::ortho(-1.f*ratio, 1.f*ratio, -1.f, 1.f, -1.f, 1.f);
	
	glm::mat4 ortho = glm::ortho(W*(-0.5f), W*(0.5f), H*(-0.5f), H*(0.5f));
	//glm::mat4 ortho = glm::ortho(0.f, W, 0.f, H);
	//return glm::perspective(glm::radians(90.0f), ratio, 0.f, 1.f)*view;
	return ortho*view;
}

void Antimatter::glx::Render2D::DrawDiamond(float x, float y, const glm::vec4& colour, const glm::vec4& glow, float scale)
{
	diamondShader.Select();
	//temp hack for transparency.  
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//probably only need to draw on one side
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TRIANGLES);
	ortho = GetOrthographicMatrix();
	//triangleShader.SetUniform1i(triangleShader.enableLightingID, enableLighting);
	diamondShader.SetUniform4x4(diamondShader.TransformID, ortho);
	/*
	float W = (float)glx::Core::windowWidth;
	float H = (float)glx::Core::windowHeight;
	float vW = 2.f;
	float vH = 2.f;
	float ratio_x = vW / W;
	float ratio_y = vH / H;
	float pixel_scale = 1.f;
	scale *= pixel_scale;
	*/
	glm::mat4 matrixA = glm::mat4(0.f);
	matrixA[0] = glm::vec4(x, y, 0.0f, 1.f);
	matrixA[1] = colour;
	matrixA[2] = glow;
	
	diamondShader.SetUniform4x4(diamondShader.vertexMatrixID, matrixA);
	diamondShader.SetUniform1f(diamondShader.scaleID, scale);
	InvokeDraw(6);
}

void Antimatter::glx::Render2D::DrawTriangle(const glm::vec2& A, const glm::vec4& colourA, const glm::vec2& B, const glm::vec4& colourB, const glm::vec2& C, const glm::vec4& colourC, int enableLighting)
{
	triangleShader.Select();

	//temp hack for transparency.  
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//probably only need to draw on one side
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TRIANGLES);
	ortho = GetOrthographicMatrix();
	triangleShader.SetUniform1i(triangleShader.enableLightingID, enableLighting);
	triangleShader.SetUniform4x4(triangleShader.TransformID, ortho);

	float pixel_scale = 1.f;

	glm::mat4 matrixA = glm::mat4(0.f);
	matrixA[0] = glm::vec4(A, 0.f, 1.f);
	matrixA[1] = colourA;
	if (colourA.w > 1.f)
		matrixA[2] = colourA;
	triangleShader.SetUniform4x4(triangleShader.vertexMatrix1ID, matrixA);

	glm::mat4 matrixB = glm::mat4(0.f);
	matrixB[0] = glm::vec4(B, 0.f, 1.f);
	matrixB[1] = colourB;
	if (colourB.w > 1.f)
		matrixB[2] = colourB;
	triangleShader.SetUniform4x4(triangleShader.vertexMatrix2ID, matrixB);

	glm::mat4 matrixC = glm::mat4(0.f);
	matrixC[0] = glm::vec4(C, 0.f, 1.f);
	matrixC[1] = colourC;
	if (colourC.w > 1.f)
		matrixC[2] = colourC;
	triangleShader.SetUniform4x4(triangleShader.vertexMatrix3ID, matrixC);
	triangleShader.SetUniform3f(triangleShader.LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z);

	InvokeDraw(3);
}

void Antimatter::glx::Render2D::EnableSingleBufferDraw()
{
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);
}

glm::vec4 Antimatter::glx::Render2D::ReadPixel(float x, float y)
{
	float values[4] = {};
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;

	//copy the multisampled buffer over
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferRead.id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferRead.id);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMS.id);
	//copy colour buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, W, H, 0, 0, W, H, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferRead.id);
	glReadPixels(x, glx::Core::windowHeight - y, 1, 1, GL_RGBA, GL_FLOAT, values);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferMS.id);
	return glm::vec4(values[0], values[1], values[2], values[3]);
}


void Antimatter::glx::Render2D::DrawBuffers()
{
	GLsizei W = (GLsizei)glx::Core::windowWidth;
	GLsizei H = (GLsizei)glx::Core::windowHeight;
		
	//copy the multisampled buffer over
	//EnableGlowBufferDraw();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer1.id);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferMS.id);
	//copy colour buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, W, H, 0, 0, W, H, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	//return;
	//copy glow buffer
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, W, H, 0, 0, W, H, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		
	//use the glow shader to blur the glow texture in one direction
	glowShader.Select();
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2.id);
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.texture2);

	glowShader.SetUniform1i(glowShader.horizontalPassID, 1);
	glowShader.SetUniform1i(glowShader.passThroughID, 1);
	glowShader.SetUniform2f(glowShader.screenSizeID, (float)glx::Core::windowWidth, (float)glx::Core::windowHeight);
	InvokeDraw(6);
	//second pass: blur in the other direction
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glowShader.Select();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, framebuffer1.texture1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, framebuffer2.texture2);

	glowShader.SetUniform1i(glowShader.horizontalPassID, 0);
	glowShader.SetUniform1i(glowShader.passThroughID, 0); //combine the two resulting textures
	glowShader.SetUniform2f(glowShader.screenSizeID, (float)glx::Core::windowWidth, (float)glx::Core::windowHeight);
	InvokeDraw(6);
	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}

void Antimatter::glx::Render2D::StartMultiBufferDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferMS.id);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glViewport(0, 0, glx::Core::windowWidth, glx::Core::windowHeight);
}
