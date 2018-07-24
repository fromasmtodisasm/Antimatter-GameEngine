#pragma once

#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "tool.h"

using namespace System;

namespace Antimatter{
	namespace Games
	{
		public ref struct Colour
		{
		public:

			float r;
			float g;
			float b;
			float a;
			Colour()
			{
				r = 0.f;
				g = 1.f;
				b = 0.7f;
				a = 1.f;
			};

			Colour(float red, float green, float blue, float alpha)
			{
				r = red;
				g = green;
				b = blue;
				a = alpha;
			}

			Colour(Colour^ colour, float alpha)
			{
				r = colour->r;
				g = colour->g;
				b = colour->b;
				a = alpha;
			}

			Colour(float red, float green, float blue)
			{
				r = red;
				g = green;
				b = blue;
				a = 1.f;
			}

			Colour(const glm::vec4& InColour)
			{
				r = InColour.x;
				g = InColour.y;
				b = InColour.z;
				a = InColour.w;
			}

			Colour^ Alpha(float value)
			{
				return gcnew Colour(r,g,b,value);
			}

			Colour^ Round()
			{
				Colour^ col = gcnew Colour();
				col->r = tool::RoundToSingleDecimal(r);
				col->g = tool::RoundToSingleDecimal(g);
				col->b = tool::RoundToSingleDecimal(b);
				col->a = tool::RoundToSingleDecimal(a);
				return col;
			}
			static bool operator ==(Colour^ a,Colour^ b)
			{
				if (a->r == b->r 
					&& a->g == b->g
					&& a->b == b->b
					&& a->a == b->a)
					return true;
				return false;
			}

			String^ ToString() override
			{
				return String::Concat("Red: ", r, " \n", "Green: ", g, " \n", "Blue: ", b, " \n", "Alpha: ", a, " \n");
			}
			
			static Colour^ RandomColour()
			{
				Random rom;
				float rr = (float)rom.NextDouble();
				float rg = (float)rom.NextDouble();
				float rb = (float)rom.NextDouble();
				return gcnew Colour(rr, rg, rb,1);
			}

			static Colour^ Make(float r, float g, float b, float a)
			{
				return gcnew Colour(r,g,b,a);
			}

			static Colour^ Make(float r, float g, float b)
			{
				return gcnew Colour(r, g, b);
			}

			static property Colour^ New
			{
				Colour^ get()
				{
					return Colour::RandomColour();
				}
			};

			static property Colour^ Red
			{
				Colour^ get()
				{
					return gcnew Colour(1.f, 0.f, 0.f);
				}
			};

			static property Colour^ Green
			{
				Colour^ get()
				{
					return gcnew Colour(0.f, 1.f, 0.f);
				}
			};

			static property Colour^ DarkGreen
			{
				Colour^ get()
				{
					return gcnew Colour(0.f, 0.5f, 0.f);
				}
			};

			static property Colour^ Blue
			{
				Colour^ get()
				{
					return gcnew Colour(0.f, 0.f, 1.f);
				}
			};

			static property Colour^ LightBlue
			{
				Colour^ get()
				{
					return gcnew Colour(0.0f, 0.8f, 1.f);
				}
			};

			static property Colour^ Yellow
			{
				Colour^ get()
				{
					return gcnew Colour(1.f, 1.f, 0.f);
				}
			};

			static property Colour^ Orange
			{
				Colour^ get()
				{
					return gcnew Colour(1.f, 0.5f, 0.f);
				}
			};

			static property Colour^ DarkOrange
			{
				Colour^ get()
				{
					return gcnew Colour(0.5f, 0.2f, 0.f);
				}
			};

			static property Colour^ Vermillion
			{
				Colour^ get()
				{
					return gcnew Colour(1.f, 0.3f, 0.f);
				}
			};

			static property Colour^ Violet
			{
				Colour^ get()
				{
					return gcnew Colour(0.5f, 0.f, 0.5f);
				}
			};

			static property Colour^ Purple
			{
				Colour^ get()
				{
					return gcnew Colour(1.f, 0.f, 1.f);
				}
			};

			static property Colour^ White
			{
				Colour^ get()
				{
					return gcnew Colour(1.f, 1.f, 1.f);
				}
			};

			static property Colour^ Black
			{
				Colour^ get()
				{
					return gcnew Colour(0.f,0.f,0.f);
				}
			};

			void Debug(String^ text)
			{
				Console::WriteLine("Colour:: "+text +"\n"+"R: " + r + " :G: " + g + " :B: " + b + " :A: " + a);
			}

			glm::vec4 glm()
			{
				return glm::vec4(r, g, b, a);
			}

			static Colour^ operator+(Colour^ col1, Colour^ col2)
			{
				Colour^ col = gcnew Colour();
				col->r = col1->r + col2->r;
				col->g = col1->g + col2->g;
				col->b = col1->b + col2->b;
				col->a = col1->a + col2->a;
				return col;
			}

			void setRGBA(float R, float G, float B, float A)
			{
				r = R;
				g = G;
				b = B;
				a = A;
			}

			void setRGB(float R, float G, float B)
			{
				r = R;
				g = G;
				b = B;
			}

			void setRGBA(Colour^ col)
			{
				r = col->r;
				g = col->g;
				b = col->b;
				a = col->a;
			}

			void setRGB(Colour^ col)
			{
				r = col->r;
				g = col->g;
				b = col->b;
			}

			static Colour^ operator-(Colour^ col1, Colour^ col2)
			{
				Colour^ col = gcnew Colour();
				col->r = col1->r - col2->r;
				col->g = col1->g - col2->g;
				col->b = col1->b - col2->b;
				col->a = col1->a - col2->a;
				return col;
			}
			/*
			static bool operator==(Colour^ col1, Colour^ col2)
			{
				if (col1 == nullptr || col2 == nullptr)
					return false;
				if (col1->r == col2->r && col1->g == col2->g && col1->b == col2->b && col1->a == col2->a)
					return true;
				return false;
			}
			*/

			static Colour^ operator*(Colour^ col1, float mult)
			{
				Colour^ col = gcnew Colour();
				col->r = col1->r * mult;
				col->g = col1->g * mult;
				col->b = col1->b * mult;
				col->a = col1->a * mult;
				return col;
			}

			static Colour^ operator/(Colour^ col1, float mult)
			{
				Colour^ col = gcnew Colour();
				col->r = col1->r / mult;
				col->g = col1->g / mult;
				col->b = col1->b / mult;
				col->a = col1->a / mult;
				return col;
			}
		};
	}
}

