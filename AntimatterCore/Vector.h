#pragma once
#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "Colour.h"
#include "TextGraphic.h"
#include "LineGraphic.h"

//https://gamedev.stackexchange.com/questions/72528/how-can-i-project-a-3d-point-onto-a-3d-line
//point line projection

namespace Antimatter {
	namespace Games {

		public ref struct Vector
		{

		public:

			float x;
			float y;
			float z;
			float w;

			Vector()
			{
				x = 0.f;
				y = 0.f;
				z = 0.f;
				w = 1.f;
			};

			Vector(const Vector% v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = v.w;
			};

			Vector(const glm::vec3& v)
			{
				x = v.x;
				y = v.y;
				z = v.z;
				w = 1.f;
			};

			Vector(float X, float Y, float Z)
			{
				x = X;
				y = Y;
				z = Z;
				w = 1.f;
			};

			Vector(float X, float Y)
			{
				x = X;
				y = Y;
				z = 0.f;
				w = 1.f;
			};

			Vector(float X, float Y, float Z, float W)
			{
				x = X;
				y = Y;
				z = Z;
				w = w;
			};

			Vector(float f)
			{
				x = f;
				y = f;
				z = f;
				w = 1.f;
			};

			Vector(Vector^ vec)
			{
				x = vec->x;
				y = vec->y;
				z = vec->z;
				w = vec->w;
			}

			Vector(const glm::vec4& vec4)
			{
				x = vec4.x;
				y = vec4.y;
				z = vec4.z;
				w = vec4.w;
			}
			Vector(const glm::vec2& vec)
			{
				x = vec.x;
				y = vec.y;
				z = 0.f;
				w = 1.f;
			}

			Colour^ AsColour()
			{
				return gcnew Colour(abs(x), abs(y), abs(z), 1.f);
			}

			Vector^ Show()
			{
				tool::show(GetString());
				return this;
			}

			Vector^ Show(String^ s)
			{
				tool::show(s,GetString());
				return this;
			}

			static void Show(glm::vec2 v2)
			{
				tool::show("X: " + v2.x.ToString(), "Y: " + v2.y.ToString());
			}

			static void Show(glm::vec3 v2)
			{
				tool::show("X: " + v2.x.ToString(), "Y: " + v2.y.ToString(), "Z: " + v2.z.ToString());
			}

			bool IsZero()
			{
				if (lengthSquared() == 0)
				{
					return true;
				}
				return false;
			}

			bool isNull()
			{
				if (this == nullptr)
					return true;
				return false;
			}

			bool isNullorZero()
			{
				if (isNull())
					return true;
				if (IsZero())
					return true;

				return false;
			}

			void Clamp(float limit)
			{
				if (x > limit)
					x = limit;

				if (y > limit)
					y = limit;

				if (z > limit)
					z = limit;

				if (x < -limit)
					x = -limit;

				if (y > -limit)
					y = -limit;

				if (z > -limit)
					z = -limit;
			}

			void Clamp(float UpperLimit,float LowerLimit)
			{
				if (x > UpperLimit)
					x = UpperLimit;

				if (y > UpperLimit)
					y = UpperLimit;

				if (z > UpperLimit)
					z = UpperLimit;

				if (x < LowerLimit)
					x = LowerLimit;

				if (y > LowerLimit)
					y = LowerLimit;

				if (z > LowerLimit)
					z = LowerLimit;
			}

			static float Distance(Vector^ v1, Vector^ v2)
			{
				return (v1 - v2)->length();
			}

			static float DistanceToPlane(Vector^ point, Vector^ normal, float dist)
			{
				float d = Vector::Dot(normal, point) - dist / Vector::Dot(normal, normal);
				return d;
			}

			static Vector^ PointOnPlane(Vector^ point, Vector^ normal, Vector^ pointPlane)
			{
				float dist = normal->dot(point);
				float d = Vector::Dot(normal, point) - dist / Vector::Dot(normal, normal);
				return point - normal*d;
			}

			String^ ToString() override
			{
				return GetString();
			}

			float distance(Vector^ v)
			{
				return (this - v)->length();
			}

			void DrawAsPointRaw(Colour^ col, float scale)
			{
				if (this == nullptr)
					return;

				gcnew LineGraphic((Vector::left*scale + this)->glm(), (Vector::right*scale + this)->glm(), col, col, false);
				gcnew LineGraphic((Vector::up*scale + this)->glm(), (Vector::down*scale + this)->glm(), col, col, false);
				gcnew LineGraphic((Vector::front*scale + this)->glm(), (Vector::back*scale + this)->glm(), col, col, false);
			}

			Vector^ DrawAs(String^ s, Colour^ col, float scale)
			{
				if (this == nullptr)
					return nullptr;
				gcnew WorldText(s, col, this->glm(), scale,scale, true);
				return this;
			}

			Vector^ DrawAs(String^ s, Colour^ col)
			{
				if (this == nullptr)
					return nullptr;
				gcnew WorldText(s, col, this->glm(), 1.f, 1.f, true);
				return this;
			}

			Vector^ DrawAs(String^ s, Colour^ col,Colour^ col2)
			{
				if (this == nullptr)
					return nullptr;
				gcnew WorldText(s, col, this->glm(), 1.f, 1.f, true);
				return this;
			}

			Vector^ DrawTo(Vector^ v,String^ s, Colour^ col,float scale)
			{
				if (this == nullptr)
					return nullptr;
				gcnew LineGraphic(this->glm(), v->glm(), col, true);
				gcnew WorldText(s, col,  v->glm(), scale,scale, true);
				return this;
			}

			Vector^ DrawTo(Vector^ v, String^ s, Colour^ col, Colour^ col2,float scale)
			{
				if (this == nullptr)
					return nullptr;
				gcnew LineGraphic(this->glm(), v->glm(), col, col2,true);
				gcnew WorldText(s, col2, v->glm(), scale, scale, true);
				return this;
			}

			Vector^ DrawTo(Vector^ v, Colour^ col)
			{
				gcnew LineGraphic(this->glm(), v->glm(), col, true);
				return this;
			}

			Vector^ DrawTo(Vector^ v, Colour^ col,Colour^ col2)
			{
				gcnew LineGraphic(this->glm(), v->glm(), col, col2,true);
				return this;
			}

			Vector ^ to2D()
			{
				return gcnew Vector(this->x, this->y, 0.f);
			}

			//https://stackoverflow.com/questions/2049582/how-to-determine-if-a-point-is-in-a-2d-triangle
			bool WithInTriangle2D(Vector^ a, Vector^ b, Vector^ c)
			{
				float as_x = this->x - a->x;
				float as_y = this->y - a->y;

				bool s_ab = (b->x - a->x)*as_y - (b->y - a->y)*as_x > 0;

				if ((c->x - a->x)*as_y - (c->y - a->y)*as_x > 0 == s_ab) return false;

				if ((c->x - b->x)*(this->y - b->y) - (c->y - b->y)*(this->x - b->x) > 0 != s_ab) return false;

				return true;
			}

			static Vector^ GetNormal2D(Vector^ A, Vector^ B)
			{
				Vector^ ab = B - A;
				ab = ab->normalized();
				return gcnew Vector(-ab->y, ab->x, 0.f);
			}

			float lengthSquared2D()
			{
				return x*x + y*y;
			}

			float length2D()
			{
				return sqrt(x*x + y*y);
			}

			float perpDot2D(Vector^ vec)
			{
				return x*vec->y - y*vec->x;
			}

			static float CrossProduct2DScaler(const Vector^% a, const Vector^% b)
			{
				return a->x * b->y - a->y * b->x;
			}

			Vector^ normalize2D()
			{
				float l = length2D();
				Vector^ v = gcnew Vector();
				v->x = this->x / l;
				v->y = this->y / l;
				return v;
			}

			static float Dot2D(Vector^ v1, Vector^ v2)
			{
				return v1->x*v2->x + v1->y*v2->y;
			}

			static Vector^ CrossProduct2DPerp(const Vector^% a, float s,bool clockwise)
			{
				if(!clockwise)
					return gcnew Vector(s * a->y, -s * a->x);
				else
					return gcnew Vector(-s * a->y, s * a->x);
			}

			//Will have to check this
			static float SignedTriangleArea2D(Vector^ a, Vector^ b, Vector^ c)
			{
				float dArea = Cross2D(b - a, c - a) * 0.5f;
				return dArea;
			}

			static float Cross2D(Vector^ a, Vector^ b)
			{
				return a->x*b->y - a->y*b->x;
			}

			static Vector^ Cross2D(float a, Vector^ v)
			{
				return gcnew Vector(-a * v->y, a * v->x);
			}

			static Vector^ Cross2D( Vector^ v, float a)
			{
				return gcnew Vector(a * v->y, -a * v->x);
			}

			static Vector^ PointToTriangleProjection2D(Vector^ a, Vector^ b, Vector^ c, Vector^ point)
			{
				//line A to B
				float Uab = 0.f;
				float Vab = 0.f;
				BarycentricCoordinates2D(a, b, point, Uab, Vab);

				//line B to C
				float Ubc = 0.f;
				float Vbc = 0.f;
				BarycentricCoordinates2D(b, c, point, Ubc, Vbc);

				//line B to C
				float Uca = 0.f;
				float Vca = 0.f;
				BarycentricCoordinates2D(c, a, point, Uca, Vca);

				if (Uab <= 0.f && Vbc <= 0.f)
				{
					return b;
				}

				if (Ubc <= 0.f && Vca <= 0.f)
				{
					return c;
				}

				if (Uca <= 0.f && Vab <= 0.f)
				{
					return a;
				}

				//now check which side of the relevant line segment the pint lies on
				Vector^ ABC = BarycentricCoordinates2DTriangle(a, b, c, point);
				float W_abc = ABC->z;
				float V_abc = ABC->y;
				float U_abc = ABC->x;

				//line AB
				if (Uab > 0.f && Vab > 0.f && W_abc <= 0.f)
				{
					return Uab*a + Vab*b;
				}

				//line BC
				if (Ubc > 0.f && Vbc > 0.f && U_abc <= 0.f)
				{
					return Ubc*b + Vbc*c;
				}

				//line CA
				if (Uca > 0.f && Vca> 0.f && V_abc <= 0.f)
				{
					return Uca*c + Vca*a;
				}

				if (U_abc > 0.f && V_abc > 0.f && W_abc > 0.f)
				{
					return point;
				}
				
				return Vector::Zero;
			}

			static Vector^ Perpendicular2D(Vector^ v1, Vector^ v2)
			{
				float dx = v2->x - v1->x;
				float dy = v2->y - v1->y;
				return gcnew Vector(-dy, dx);
			}

			static Vector^ BarycentricCoordinates2DTriangle(Vector^ a, Vector^ b,Vector^ c,Vector^ q)
			{
				 float Area = SignedTriangleArea2D(a, b, c);
				 //TODO check the ordering here
				 float AreaQBC = SignedTriangleArea2D(q, b, c);
				 float AreaQCA = SignedTriangleArea2D(q, c, a);
				 float AreaQAB = SignedTriangleArea2D(q, a, b);

				 float u = AreaQBC/Area;
				 float v = AreaQCA/Area;
				 float w = AreaQAB/Area;
				
				 return gcnew Vector(u, v, w);
			}

			static Vector^  BarycentricCoordinates2D(Vector^ A, Vector^ B, Vector^ point,float% U,float% V)
			{
				Vector^ p = Vector::ProjecToLine2D(A, B, point);
				Vector^ AB = B - A;
				float L = AB->lengthSquared2D();
				V = ((p - A)->dot(AB) / L);
				U = 1.f - V;

				if (V <= 0.f)
					p = A;
				if (U <= 0.f)
					p = B;

				return p;
			}

			Vector^ PointToLine3D(Vector^ a, Vector^b, Vector^ point)
			{
				Vector^ ab = (b - a)->normalized();
				Vector^ ap = point - a;
				Vector^ proj = a + (ab*ap->dot(ab));
				float u = 0.f;
				float v = 0.f;
				proj = Vector::BarycentricCoordinates3D(a, b, proj, u, v);

				return proj;
			}

			static Vector^ BarycentricCoordinates3D(Vector^ A, Vector^ B, Vector^ point, float% U, float% V)
			{
				Vector^ AB = B - A;
				float L = AB->length();
				V = ((point - A)->dot(AB) / L);
				U = 1.f - V;

				if (V <= 0.f)
					point = A;
				if (U <= 0.f)
					point = B;

				return point;
			}

			//http://www.sunshine2k.de/coding/java/PointOnLine/PointOnLine.html
			static Vector^ ProjecToLine2D(Vector^ v1,Vector^ v2,Vector^ q)
			{
				Vector^ e1 = v2-v1;
				Vector^ e2 = q-v1;
				float value = e1->dot2d(e2);
				float length = e1->x*e1->x + e1->y*e1->y;

				Vector^ point = gcnew Vector(v1->x + (value*e1->x) / length, v1->y + (value*e1->y) / length);
				return point;
			}

			//http://www.sunshine2k.de/coding/java/PointOnLine/PointOnLine.html
			static Vector^ ProjecToPlane2D(Vector^ v1, Vector^ v2, Vector^ q)
			{
				Vector^ e1 = v2 - v1;
				Vector^ e2 = q - v1;
				float value = e1->dot2d(e2);
				float length = e1->x*e1->x + e1->y*e1->y;

				Vector^ point = gcnew Vector(v1->x + (value*e1->x) / length, v1->y + (value*e1->y) / length);
				return point;
			}

			//needs fixing
			void Display(String ^text,float scale)
			{
				if (this == nullptr)
					return;
				glm::vec3 pos = this->glm();
				FontManager::DrawInWorld(text, pos, Colour::White, scale, scale, glm::mat4(1.f), true);
				float xo = 0.f;
				FontManager::DrawInWorld(" X " + this->x.ToString(), pos + glm::vec3(0, -0.2f*scale,0), gcnew Colour(1, 0.2f, 0.2f, 1), scale, scale,glm::mat4(1.f), true);
				FontManager::DrawInWorld(" Y " + this->y.ToString(), pos + glm::vec3(0, -0.4f*scale, 0), gcnew Colour(0.2f, 1, 0.2f, 1), scale, scale, glm::mat4(1.f),true);
				FontManager::DrawInWorld(" Z " + this->z.ToString(), pos + glm::vec3(0, -0.6f*scale, 0), gcnew Colour(0.2f, 0.2f, 1, 1), scale, scale, glm::mat4(1.f),true);
			}

			void DisplayNDC(String ^text, float scale,float x, float y)
			{
				if (this == nullptr)
					return;
				FontManager::current->RenderString(text, 0.f, 0.f, Colour::White, scale, scale);
				float xo = -text->Length / 4.f;
				FontManager::current->RenderString(" X " + this->x.ToString(), x, -0.2f*1.f/scale, gcnew Colour(1.f, 0.2f, 0.2f, 1.f), scale, scale);
				FontManager::current->RenderString(" Y " + this->y.ToString(), x, -0.4f*1.f/scale, gcnew Colour(0.2f, 1.f, 0.2f, 1.f), scale, scale);
				FontManager::current->RenderString(" Z " + this->z.ToString(), x, -0.6f*1.f/scale, gcnew Colour(0.2f, 0.2f, 1.f, 1.f), scale, scale);
			}

			void Display(String ^text)
			{
				if (this == nullptr)
					return;
				Display(text, 1);
			}

			void DisplayAs(String ^text,Colour^ col,float scale)
			{
				if (this == nullptr)
					return;
				FontManager::DrawInWorld(text, this->glm(), col, scale, scale, glm::mat4(1.f), true);
			}

			void DisplayAs(String ^text, Colour^ col)
			{
				if (this == nullptr)
					return;
				FontManager::DrawInWorld(text, this->glm(), col, 1.f, 1.f, glm::mat4(1.f), true);
			}

			void DisplayAs(String ^text)
			{
				if (this == nullptr)
					return;
				FontManager::DrawInWorld(text, this->glm(), Colour::White, 1.f, 1.f, glm::mat4(1.f), true);
			}

			void DisplayAs(String ^text,float scale)
			{
				if (this == nullptr)
					return;
				FontManager::DrawInWorld(text, this->glm(), Colour::White, scale,scale, glm::mat4(1.f), true);
			}

			void DisplayAt(String ^text, Vector^ point, float scale)
			{
				if (text == nullptr || point == nullptr)
					return;
				FontManager::DisplayAtPoint(text, this->glm(), point->glm(), scale);
			}

			void Display(String ^text,Colour^ colour,float scale)
			{
				glm::vec4 c = colour->glm();
				glm::mat4 mat = glm::mat4(1.f);
				int row = 0;
				float space = 100.f;
				FontManager::current->RenderNative(text, x, y, z, c, scale, scale, true, true, mat);
				FontManager::current->RenderNative("	X: "+x, x, y-(1.f*space), z, c, scale, scale, true, true, mat);
				FontManager::current->RenderNative("	Y: " + y, x, y-(2.f*space), z, c, scale, scale, true, true, mat);
				FontManager::current->RenderNative("	Z: " + z, x, y-(3.f*space), z, c, scale, scale, true, true, mat);
			}

			static float Area(Vector^ a, Vector^ b, Vector^ c)
			{
				Vector^ area = Vector::Cross(a, b);
				area += Vector::Cross(b, c);
				area += Vector::Cross(c, a);
				area /= 2.f;
				return area->length();
			}

			static float AreaSigned(Vector^ a, Vector^ b, Vector^ c,Vector^ N)
			{
				Vector^ area = Vector::Cross(a, b);
				area += Vector::Cross(b, c);
				area += Vector::Cross(c, a);
				area /= 2.f;
				return Vector::Dot(area,N);
			}

			static glm::mat3 MultiplyMatrix(glm::mat3 mat, float f)
			{
				glm::mat3 m(1.f);
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
						m[i][j] = mat[i][j] * f;
				}
				return glm::mat3(m);
			}

			Vector^ W(float value)
			{
				return gcnew Vector(x,y,z,1.f);
			}

			static glm::mat3 CrossProductMatrix(Vector^ v)
			{
				glm::mat3 mat = glm::mat3();
				mat[0][1] = -v->z;
				mat[0][2] = v->y;
				mat[1][0] = v->z;
				mat[1][2] = -v->x;
				mat[2][0] = -v->y;
				mat[2][1] = v->x;
				return mat;
			}

			static void DebugMatrix(glm::mat3& it, String^ text)
			{
				if (text != nullptr)
					tool::debug(1,text);

				cout << "\n ** mat3 **" << endl;
				cout << it[0][0] << " : " <<
					it[0][1] << " : " <<
					it[0][2] << endl;
				cout << it[1][0] << " : " <<
					it[1][1] << " : " <<
					it[1][2] << endl;
				cout << it[2][0] << " : " <<
					it[2][1] << " : " <<
					it[2][2] << endl;
				cout << endl;
			}

			static Vector^ Random()
			{
				System::Random rom;
				double MIN_VALUE = -20.0;
				double MAX_VALUE = -10.0;
				double range = -rom.NextDouble();
				float X = (float)(rom.NextDouble()+range);
				float Y = (float)(rom.NextDouble()+range);
				float Z = (float)(rom.NextDouble()+range);
				Vector^ v = gcnew Vector(X, Y, Z);
				return v;
			}

			static Vector^ RandomPosition()
			{
				System::Random^ rom = gcnew System::Random();
				float rr = (float)rom->NextDouble();
				float rg = (float)rom->NextDouble();
				float negate = (float)rom->NextDouble() * 10.f;
				float rb = (float)rom->NextDouble();
				
				return gcnew Vector((rr-negate)+2.f, rg*1.5f, rb-negate)*3.f;
			}

			static Vector^ Subtract(Vector^% a, Vector^% b, Vector^% out)
			{
				out = a - b;
				return out;
			}

			static Vector^ Zero = gcnew Vector(0.f, 0.f, 0.f);
			static Vector^ up = gcnew Vector(0.f, 1.f, 0.f);
			static Vector^ down = gcnew Vector(0.f, -1.f, 0.f);
			static Vector^ left = gcnew Vector(-1.f, 0.f, 0.f);
			static Vector^ right = gcnew Vector(1.f, 0.f, 0.f);
			static Vector^ front = gcnew Vector(0.f, 0.f, 1.f);
			static Vector^ back = gcnew Vector(0.f, 0.f, -1.f);

			static Vector^ Min(Vector^ v1, Vector^ v2)
			{
				return gcnew Vector(Math::Min(v1->x, v2->x), Math::Min(v1->y, v2->y), Math::Min(v1->z, v2->z));
			}

			static Vector^ Max(Vector^ v1, Vector^ v2)
			{
				return gcnew Vector(Math::Max(v1->x, v2->x), Math::Max(v1->y, v2->y), Math::Max(v1->z, v2->z));
			}

			static List<Vector^>^ ToVectorList(cli::array<float>^ points)
			{
				int vi = 0; //vert index
				List<Vector^>^ list = gcnew List<Vector^>();
				for (int i = 0; i < points->Length/3; i++) //count in mutiples of three
				{
					Vector^ v = gcnew Vector();
					v->x = points[vi];
					v->y = points[vi + 1];
					v->z = points[vi + 2];
					list->Add(v);
					vi += 3;
				}
				return list;
			}

			static float DistToSegment(Vector^ point, Vector^ s1, Vector^ s2)
			{
				Vector^ v = s2 - s1;
				Vector^ w = point - s1;

				float c1 = Vector::Dot(w, v);
				if (c1 <= 0)
					return Vector::Distance(point, s2);

				float c2 = Vector::Dot(v, v);
					return Vector::Distance(point, s1);

				float b = c1 / c2;
				Vector^Pb = s1 + b * v;
				return Vector::Distance(point, Pb);
			}

			//https://stackoverflow.com/questions/7168484/3d-line-segment-and-plane-intersection
			static bool LinePlaneIntersection(Vector^% contact, Vector^ ray, Vector^ rayOrigin,
				Vector^ normal, Vector^ coord)
			{
				// get d value
				Vector^ rayDir = ray - rayOrigin;
				float d = Dot(normal, coord);

				if (fabs(d) < 0.000001f)
				{
					tool::debugError("No intersection here");
					return false;
				}

				Vector^ w = ray - coord;
				float si = -normal->dot(w) / d;
				Vector^ psi = w + si*rayDir + coord;
				contact = psi;
				return true;
			}
		
			static bool InsideTriangle(Vector^ point, Vector^ a, Vector^ b, Vector^ c)
			{
				Vector^ U = b - a;
				Vector^ V = c - a;
				Vector^ p = point- a;
				float dotU = p->dot(U);
				float dotV = p->dot(U);
				
				if (dotU <= 1 && dotV <= 1 && dotU >= 0 && dotV >= 0)
				{
					if (dotU + dotV <= 1)
						return true;
				}
				return false;
			}

			static bool IsNearZero(float f)
			{
				//use .net or native?
				if(Math::Abs(f) <= .000000001f)
					return true;
				else
					return false;
			}

			bool IsNearZero()
			{
				 if (IsNearZero(x) && IsNearZero(y) && IsNearZero(z))
					 return true;
				 return false;
			}

			static void debugVec(glm::vec3 vec)
			{
				cout << "X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << endl;
			}

			static void debugVec(glm::vec4 vec)
			{
				cout << "X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << " W: "<<vec.w<<endl;
			}
			
			String^ GetString()
			{
				if (this == nullptr)
					return " null ";
				return String::Concat(" Vector:-\nX: ", x , " \nY: " , y , " \nZ: " , z);
			}

			Vector^ See(float scale)
			{
				this->DrawAsPointRaw(Colour::New, scale);
				return this;
			}

			void debugVec(String^ text)
			{
				Console::WriteLine("\n"+text + "\n" + GetString());
			}

			//direction's remove the translation/position, so 0 out the homogenours coordinates
			Vector^ asDirection()
			{
				return gcnew Vector(x, y, z, 0);
			}

			//points keep the translation/position (1)
			Vector^ asPoint()
			{
				return gcnew Vector(x, y, z, 1);
			}

			glm::vec3 glm()
			{	
				float X = x;
				float Y = y;
				float Z = z;
				return glm::vec3(X,Y,Z);
			}

			glm::vec4 ToGLM4(float W)
			{
				return glm::vec4(x, y, z,W);
			}
			glm::vec4 ToGLM4()
			{
				return glm::vec4(x, y, z, w);
			}

			glm::vec2 ToGLM2()
			{
				float X = x;
				float Y = y;
				return glm::vec2(X,Y);
			}
			glm::vec3 ToGLM3()
			{
				float X = x;
				float Y = y;
				float Z = z;
				return glm::vec3(X, Y,Z);
			}

			void debugVec()
			{

				debugVec("Debug Vec");
			}

			static operator glm::vec3(Vector^ vector)
			{
				glm::vec3 gvec;
				gvec.x = vector->x;
				gvec.y = vector->y;
				gvec.z = vector->z;
				return gvec;
			}

			float dot(Vector^ vector)
			{
				return Dot(this, vector);
			}

			float dot2d(Vector^ vector)
			{
				return this->x * vector->x + this->y * vector->y;
			}

			static float Dot(Vector^ vector1, Vector^ vector2)
			{
				return vector1->x * vector2->x + vector1->y * vector2->y + vector1->z * vector2->z;
			}

			static float SideOfLine(Vector^ a, Vector^ b, Vector^ p)
			{
				return (b->x - a->x) * (p->y - a->y) - (b->y - a->y) * (p->x - a->x);
			}
			
			// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
			// intersect the intersection point may be stored in the floats i_x and i_y.
			static float LineIntersection(Vector^ p0, Vector^ p1,
				Vector^ p2, Vector^ p3, Vector^% i)
			{
				float s1_x, s1_y, s2_x, s2_y;
				s1_x = p1->x - p0->x;     s1_y = p1->y - p0->y;
				s2_x = p3->x - p2->x;     s2_y = p3->y - p2->y;

				float s, t;
				s = (-s1_y * (p0->x - p2->x) + s1_x * (p0->y - p2->y)) / (-s2_x * s1_y + s1_x * s2_y);
				t = (s2_x * (p0->y - p2->y) - s2_y * (p0->x - p2->x)) / (-s2_x * s1_y + s1_x * s2_y);

				//remove = for co linear case
				if (s > 0 && s < 1 && t > 0 && t < 1)
				{
					if (i == nullptr)
						i = gcnew Vector();
					i->x = p0->x + (t * s1_x);
					i->y = p0->y + (t * s1_y);
					return 1;
				}

				return 0; // No collision
			}

			static float DistanceToLine(Vector^ a, Vector^ b, Vector^ p)
			{
				Vector^ n = b - a;
				Vector^ pa = a - p;
				Vector^ c = n*(Vector::Dot2D(pa, n) / Vector::Dot2D(n, n));
				Vector^ d = pa - c;
				return sqrt(Vector::Dot2D(d, d));
			}

			static bool SideOfPlane(Vector^ normal, Vector^ plane, Vector^ point)
			{
				Vector^ line = point - plane;
				float dot = line->dot2d(normal);
				if (dot <= 0.f)
					return true;
				return false;
			}

			//doesn't seem to work
			static Vector^ TriangleNormal3D(Vector^ p1, Vector^ p2, Vector^ p3)
			{
				Vector^ u = p1 - p2;
				Vector^ v = p1 - p3;
				return Vector::Cross(u, v);
			}

			static bool InsidePlane3d(Vector^ normal, Vector^ plane, Vector^ point)
			{
				Vector^ line = point - plane;
				float dot = normal->dot(line);
				if (dot > 0.f)
					return true;
				return false;
			}

			static float DistanceToPlane3d(Vector^ normal, Vector^ plane, Vector^ point)
			{
				Vector^ line = point - plane;
				float dot = normal->dot(line);
				return dot;
			}

			static Vector^ ComputeTangent(Vector^ direction1,Vector^ N)
			{
				tool::show("22 oct 17:  just removed negation of direction.  Check the calling function to ensure the direction is signed correctly");
				return Vector::Cross(direction1, N*N);
			}

			Vector^ cross(Vector^ v2)
			{
				return Vector::Cross(this, v2);
			}

			static Vector^ RayPlaneIntersection(Vector^ edgeA, Vector^ edgeB,Vector^ planePoint,Vector^ normal)
			{
				Vector^ diff = planePoint - edgeA;
				Vector^ ev = (edgeB - edgeA)->normalized();  //check that these be in the same direction
				float d1 = diff->dot(normal);
				float d2 = ev->dot(normal);
				float d3 = d1 / d2;

				if (d3 < 0.f)
				{
					return nullptr;
				}
				return edgeA + ev*d3;
			}

			static bool OutsidePlane(Vector^ normal,  Vector ^ point)
			{
				float dist = point->dot(normal);

				if (dist > 0.f)
					return true;
				if (dist == 0.f)
					return false;

				return false;
			}

			static Vector^ Cross(Vector^ v1, Vector^ v2)
			{
				return gcnew Vector(v1->y*v2->z - v1->z*v2->y, v1->z*v2->x - v1->x*v2->z, v1->x*v2->y - v1->y*v2->x);
			}

			static Vector^ DoubleCross(Vector^ v1, Vector^ v2)
			{
				return Cross(Cross(v1, v2), v1);
			}

			static Vector^ TripleCross(Vector^ a, Vector^ b, Vector^ c)
			{
				return Vector::Cross(a, Vector::Cross(b, c));
			}

			static float Angle(Vector^ vec1, Vector^ vec2)
			{
				return acos(Vector::Dot(vec1->normalized(), vec2->normalized()));
			}

			static operator Vector ^ (glm::vec3 gvec)
			{
				return gcnew Vector(gvec.x, gvec.y, gvec.z);
			}

			static operator Vector ^ (Vector vec)
			{
				return gcnew Vector(vec.x, vec.y, vec.z);
			}

			static Vector^ operator*(const glm::mat4& mat, Vector^ vector)
			{
				return gcnew Vector(mat*glm::vec4(vector->x, vector->y, vector->z, vector->w));
			}

			static Vector^ operator*(Vector^ vector,const glm::mat4& mat)
			{
				return gcnew Vector(glm::vec4(vector->x, vector->y, vector->z, vector->w)*mat);
			}

			static Vector^ operator*(glm::mat4* mat, Vector^ vector)
			{
				return gcnew Vector((*mat)*glm::vec4(vector->x, vector->y, vector->z, vector->w));
			}

			static Vector^ operator*(Vector^ vector, glm::mat4* mat)
			{
				return gcnew Vector(glm::vec4(vector->x, vector->y, vector->z, vector->w)*(*mat));
			}

			static Vector^ operator-(const Vector^ vec1, const Vector^ vec2)
			{
				Vector^ v = gcnew Vector();
				v->x = vec1->x - vec2->x;
				v->y = vec1->y - vec2->y;
				v->z = vec1->z - vec2->z;
				return v;
			}

			Vector^ Negated()
			{
				return gcnew Vector(-this->x, -this->y, -this->z);
			}

			static Vector^ operator -(Vector^ v)
			{
				return v->Negated();
			}

			static Vector^ operator /(Vector^ a, Vector^ b)
			{
				return Vector::Divide(a, b);
			}
			static Vector^ operator /(Vector^ a, float b)
			{
				return Vector::Divide(a, b);
			}

			static Vector^ Divide(Vector^ value1, Vector^ value2)
			{
				Vector^ v = gcnew Vector();
				v->x = value1->x/ value2->x;
				v->y = value1->y / value2->y;
				v->z = value1->z / value2->z;
				return v;
			}

			static Vector^ Divide(Vector^ value1, float value2)
			{
				Vector^ v = gcnew Vector();
				float factor = 1.f / value2;
				v->x = value1->x * factor;
				v->y = value1->y * factor;
				v->z = value1->z * factor;
				return v;
			}


			static Vector^ operator%(Vector^ vec1, Vector^ vec2)
			{
				return vec1->cross(vec2);
			}

			static float operator^(Vector^ vec1, Vector^ vec2)
			{
				return vec1->dot(vec2);
			}
			
			static Vector^ operator*(Vector^ vec1, Vector^ vec2)
			{
				Vector^ vec = gcnew Vector(vec1->x*vec2->x, vec1->y*vec2->y, vec1->z*vec2->z);
				return vec;
			}

			static Vector^ operator+(const Vector^ vec1, const Vector^ vec2)
			{
				Vector^ v = gcnew Vector();
				v->x = vec1->x + vec2->x;
				v->y = vec1->y + vec2->y;
				v->z = vec1->z + vec2->z;
				return v;
			}

			static Vector^ operator*(Vector^ vec1, float mult)
			{
				Vector^ v = gcnew Vector();
				v->x = vec1->x *mult;
				v->y = vec1->y *mult;
				v->z = vec1->z *mult;
				return v;
			}

			static Vector^ operator*( float mult, Vector^ vec1)
			{
				Vector^ v = gcnew Vector();
				v->x = vec1->x *mult;
				v->y = vec1->y *mult;
				v->z = vec1->z *mult;
				return v;
			}

			float lengthSquared()
			{
				return x*x + y*y + z*z;
			}

			float length()
			{
				return sqrt(x*x + y*y + z*z);
			}
			
			property Vector^ n
			{
				Vector^ get()
				{
					return normalized();
				};
			}

			property float m
			{
				float get()
				{
					return length();
				};
			}

			property float m2
			{
				float get()
				{
					return lengthSquared();
				};
			}

			Vector^ normalized()
			{
				Vector^ v = gcnew Vector();
				float l = length();
				v->x = x/l;
				v->y = y/l;
				v->z = z/l;
				return v;
			}

			void normalize()
			{
				float l = length();
				x = x / l;
				y = y / l;
				z = z / l;
			}

			static float Det(const Vector^ v0, const Vector^ v1, const Vector^ v2)
			{
				float det =
					v0->x * (v1->y * v2->z - v1->z * v2->y) +
					v0->y * (v1->z * v2->x - v1->x * v2->z) +
					v0->z * (v1->x * v2->y - v1->y * v2->x);
				return det;
			}

			//haven't checked these yet
			static Vector^ operator*(Vector^ vector, const glm::mat3& mat)
			{
				return gcnew Vector(vector->glm()*mat);
			}

			static Vector^ operator*(Vector^ vector, glm::mat3* mat)
			{
				return gcnew Vector(vector->glm()*(*mat));
			}

			static Vector^ operator*(const glm::mat3& mat, Vector^ vector)
			{
				return gcnew Vector(mat*vector->glm());
			}

			static Vector^ operator*(glm::mat3* mat,Vector^ vector)
			{
				return gcnew Vector((*mat)*vector);
			}
			
			float Distance( Vector^ value2)
			{
				return sqrt((this->x - value2->x) * (this->x - value2->x) +
					(this->y - value2->y) * (this->y - value2->y) +
					(this->z - value2->z) * (this->z - value2->z));
			}

			static Vector^ Lerp(Vector^ A, Vector^ B,float alpha)
			{
				Vector^ out_v = alpha*A + (1 - alpha)*B;
				return out_v;
			}

			static float DistanceSquared(Vector^ value1, Vector^ value2)
			{
				return (value1->x - value2->x) * (value1->x - value2->x) +
					(value1->y - value2->y) * (value1->y - value2->y) +
					(value1->z - value2->z) * (value1->z - value2->z);
			}
		};
	}
}