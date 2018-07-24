#pragma once
#include "glaxShader.h"
#include "tool.h"

namespace Antimatter {
	namespace glx {

		class ShaderFor2DGlow : public Shader
		{
		public:
			ShaderFor2DGlow()
			{
				vertexString = vertexShader;
				geometryString = triangleGeometryShader;
				fragmentString = fragmentShader;
				name = "Glow ";
			}
			const char* vertexShader = R"(#version 420
			void main()
			{
				
			}
			)";

			const char* triangleGeometryShader = R"(
			#version 420

			layout(triangles) in;
			layout(triangle_strip, max_vertices=6) out;

			out fData
			{
				vec4 pos;
				vec4 color;
				vec2 uv;
			}frag;

			//fullscreen quad
			void main() 
			{
				frag.uv = vec2( 1.0, 1.0 );
				frag.color = vec4(1,1,1,1);
				frag.pos = vec4( 1.0, 1.0, 0.0, 1.0 );
				gl_Position = frag.pos;
				EmitVertex();

				frag.uv = vec2( 0.0, 1.0 ); 
				frag.color = vec4(1,0,0,1);
				frag.pos = vec4(-1.0, 1.0, 0.0, 1.0 );
				gl_Position = frag.pos;
				EmitVertex();

				frag.uv = vec2( 1.0, 0.0 ); 
				frag.color = vec4(0,0,1,1);
				frag.pos = vec4( 1.0,-1.0, 0.0, 1.0 );
				gl_Position = frag.pos;
				EmitVertex();

				frag.uv = vec2( 0.0, 0.0 );
				frag.color = vec4(0,1,0,1);
				frag.pos = vec4(-1.0,-1.0, 0.0, 1.0 );
				gl_Position = frag.pos; 
				EmitVertex();
			}

			//for drawing lines in clip space
			//http://stackoverflow.com/questions/12758192/wide-lines-in-geometry-shader-behaves-oddly
			)";

			//https://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/lighting.php
			//https://learnopengl.com/Lighting/Basic-Lighting
			const char * fragmentShader = R"(
			#version 420

			out vec4 outcolour;
			out vec4 glowcolour;
			uniform vec3 lightposition;
			uniform vec4 lightintensity;
			uniform mat4 transform;
			uniform int enableLighting;
			layout(binding=0) uniform sampler2D tex;
			layout(binding=1) uniform sampler2D glowTex;
			uniform vec2 screenSize;
			uniform int horizontalPass;
            uniform int passThrough;
 
		const float pi = 3.14159265;			

			in fData
			{
				vec3 pos;
				vec4 color;
				vec2 uv;
			}frag;

			vec4 Blur2()
			{
				// Current texture coordinate
				vec2 texel = vec2(gl_FragCoord.xy/screenSize);
				vec4 pixel = vec4(texture(tex, texel));
				float sigma = 3.0;
				// Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
				vec3 incrementalGaussian;
				incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
				incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
				incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
				for(int i=1;i<10;i++)
				{
					pixel +=vec4(texture(tex, texel+vec2(i/screenSize.x,0.0)))*incrementalGaussian.x;
					pixel += vec4(texture(tex, texel+vec2(-i/screenSize.x,0.0)))*incrementalGaussian.x;
					//pixel +=vec4(texture(tex, texel+vec2(0.0,i/screenSize.y)))*incrementalGaussian.x;
					//pixel += vec4(texture(tex, texel+vec2(0.0,-i/screenSize.y)))*incrementalGaussian.x;
				incrementalGaussian.xy *= incrementalGaussian.yz;
				}
				return pixel;


				float pixelWidth = 1.0;
				float pixelHeight = 1.0;
				// Larger constant = bigger glow
				float glow = 4.0 * ((pixelWidth + pixelHeight) / 2.0);
				// The vector to contain the new, "bloomed" colour values
				vec4 bloom = vec4(0);

				// Loop over all the pixels on the texture in the area given by the constant in glow
				int count = 0;

				for(float x = texel.x - glow; x < texel.x + glow; x += pixelWidth)
				{
					for(float y = texel.y - glow; y < texel.y + glow; y += pixelHeight)
					{
						// Add that pixel's value to the bloom vector
						bloom += (texture(tex, vec2(x, y)) - 0.4) * 30.0;

						// Add 1 to the number of pixels sampled
						count++;
					}
				}

				// Divide by the number of pixels sampled to average out the value
				// The constant being multiplied with count here will dim the bloom effect a bit, with higher values
				// Clamp the value between a 0.0 to 1.0 range

				//bloom = clamp(bloom / (count * 30), 0.0, 1.0);
				// Set the current fragment to the original texture pixel, with our bloom value added on

				return bloom;
			}

			vec4 Blur(vec4 inCol)
			{
				vec2 texOffset = -screenSize;
				int hP = horizontalPass;
				float sigma = 5.0+(inCol.a*2.0);
				float blurSize = 500.0;
				float numBlurPixelsPerSide = float(blurSize / 2.0); 
				
				vec2 blurMultiplyVec = 0 < hP ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
 
				// Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
				vec3 incrementalGaussian;
				incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma);
				incrementalGaussian.y = exp(-0.5 / (sigma * sigma));
				incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
 
				vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
				float coefficientSum = 0.0;
 
				// Take the central sample first...
				avgValue += texture(glowTex, gl_FragCoord.xy/screenSize) * incrementalGaussian.x;
				coefficientSum += incrementalGaussian.x;
				incrementalGaussian.xy *= incrementalGaussian.yz;
 
				// Go through the remaining 8 vertical samples (4 on each side of the center)
				for (float i = 1.0; i <= numBlurPixelsPerSide; i++)
				{ 
					avgValue += texture(glowTex, gl_FragCoord.xy/screenSize - i/screenSize*
											blurMultiplyVec) * incrementalGaussian.x;         
					avgValue += texture(glowTex, (gl_FragCoord.xy/screenSize) + (i/screenSize*
											blurMultiplyVec)) * incrementalGaussian.x;         
					coefficientSum += 2.0 * incrementalGaussian.x;
					incrementalGaussian.xy *= incrementalGaussian.yz;
				}
 
				return avgValue / coefficientSum;
			}

			void main()
			{
				//Check if we should be using GL fragcoord
				vec4 finalColour = texture(tex,gl_FragCoord.xy/screenSize);
				vec4 glowColour = texture(glowTex,gl_FragCoord.xy/screenSize);
				//insert glow here
				vec4 b2 = Blur(glowColour);
				//manual gamma correction
				//finalColour.rgb = pow(finalColour.rgb,vec3(1.0/2.2));
				//finalColour.a = 1.0;
				if(passThrough == 1)
				{
					outcolour = finalColour;
					glowcolour = b2;
				}
				if(passThrough == 0)
				{
					outcolour = finalColour + b2;
				}
			}

			
			//2D lighting
			//https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
			//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
			)";

			

			unsigned int TransformID;
			unsigned int LightPositionID;
			unsigned int LinePointA;
			unsigned int LinePointB;
			unsigned int LinePointC;
			unsigned int LineColourA;
			unsigned int LineColourB;
			unsigned int LineColourC;
			unsigned int enableLightingID;
			unsigned int thicknessID;
			unsigned int screenSizeID;
			unsigned int diffuseTextureID;
			unsigned int glowTextureID;
			unsigned int horizontalPassID;
			unsigned int passThroughID;
			

			//managed to native transition
			//watch out for thunking
			virtual void GetUniforms() override
			{
				//	glUseProgram(id); //delete this line
				TransformID = glGetUniformLocation(id, "transform");
				LightPositionID = glGetUniformLocation(id, "lightposition");
				LinePointA = glGetUniformLocation(id, "PointA");
				LinePointB = glGetUniformLocation(id, "PointB");
				LinePointC = glGetUniformLocation(id, "PointC");
				LineColourA = glGetUniformLocation(id, "colourA");
				LineColourB = glGetUniformLocation(id, "colourB");
				LineColourC = glGetUniformLocation(id, "colourC");
				enableLightingID = glGetUniformLocation(id, "enableLighting");
				thicknessID = glGetUniformLocation(id, "thickness");
				screenSizeID = glGetUniformLocation(id, "screenSize");
				diffuseTextureID = glGetUniformLocation(id, "tex");
				glowTextureID = glGetUniformLocation(id, "glowTex");
				horizontalPassID = glGetUniformLocation(id, "horizontalPass");
				passThroughID = glGetUniformLocation(id, "passThrough");
			}

			virtual void Select() override
			{
				Shader::Select();
				//glDisable(GL_CULL_FACE);
			}
		};
	}
}