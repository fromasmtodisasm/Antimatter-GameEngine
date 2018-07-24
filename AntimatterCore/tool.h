#pragma once
#include "SystemHeader.h" //<- prevent win.h/c#form conflicts
#include "GlaxionHeader.h"
//#include <msclr\marshal_cppstd.h>
using namespace System::Runtime::InteropServices;
using namespace System::Diagnostics;
//using namespace msclr::interop;

namespace Antimatter {
	namespace Games{

		public ref class tool
		{
		public:
			tool() {};
			
			/*
			static char* ToCString(String^ s)
			{
				marshal_context^ context = gcnew marshal_context();
				char* udata = const_cast<char*>(context->marshal_as<const char*>(s));
				return udata;
			}

			static const char* ToConstCString(String^ s)
			{
				marshal_context^ context = gcnew marshal_context();
				const char* udata = const_cast<char*>(context->marshal_as<const char*>(s));
				return udata;
			}
			*/

			static void SetConsoleColours(ConsoleColor foreground, ConsoleColor background)
			{
				Console::ForegroundColor = foreground;
				Console::BackgroundColor = background;
			}

			static bool FilePathHasInvalidChars(String^ path)
			{

				return (!String::IsNullOrEmpty(path) && path->IndexOfAny(System::IO::Path::GetInvalidPathChars()) >= 0);
			}

			static float clamp(float value, float min, float max)
			{
				if (value > max)
					return max;
				if (value < min)
					return min;
				return value;
			}

			static double clamp(double value, double min, double max)
			{
				if (value > max)
					return max;
				if (value < min)
					return min;
				return value;
			}

			static int clamp(int value, int min, int max)
			{
				if (value > max)
					return max;
				if (value < min)
					return min;
				return value;
			}

			static long clamp(long value, long min, long max)
			{
				if (value > max)
					return max;
				if (value < min)
					return min;
				return value;
			}

			static float RoundToSingleDecimal(float num)
			{
				//  return (float)Math.Floor(num * 10 + 0.5) / 10;
				return (float)Math::Round(num, 1, MidpointRounding::ToEven);
			}

			static float Min(float num1, float num2)
			{
				// Tool.Show(num1, num2);
				if (num1 < num2)
					return num1;
				else
					return num2;
			}

			static float Max(float num1, float num2)
			{
				if (num1 > num2)
					return num1;
				else
					return num2;
			}

	
			static void WriteLine(ConsoleColor col, ...cli::array<String^>^ text)
			{
				ConsoleColor c = Console::ForegroundColor;
				Console::ForegroundColor = col;
				for (int i = 0; i < text->Length; i++)
					Console::WriteLine(text[i]);
				Console::ForegroundColor = c;

			}



			static int Count;

			static void DebugText(...cli::array<String^>^ text)
			{
				for (int i = 0; i < text->Length; i++)
				{
					Console::WriteLine(text[i]);
				}
			}

			static float min(float a, float b)
			{
				if (a < b)
					return a;
				return b;
			}

			static float max(float a, float b)
			{
				if (a > b)
					return a;
				return b;
			}

			static int min(int a, int b)
			{
				if (b < a)
					return b;
				return a;
			}

			static int max(int a, int b)
			{
				if (b > a)
					return b;
				return a;
			}

			static double min(double a, double b)
			{
				if (b < a)
					return b;
				return a;
			}

			static double max(double a, double b)
			{
				if (b > a)
					return b;
				return a;
			}

			static long min(long a, long b)
			{
				if (b < a)
					return b;
				return a;
			}

			static long max(long a, long b)
			{
				if (b > a)
					return b;
				return a;
			}

			static void debug(ConsoleColor colour, ...cli::array<Object^>^ text)
			{
				ConsoleColor c = Console::ForegroundColor;
				Console::ForegroundColor = colour;

				//  debugTrace = 7;
				debug(3, text);
				//  debugTrace = 2;

				Console::ForegroundColor = c;

			}

			static void debugError(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::Red, text);
			}

			static void debugGreen(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::Green, text);
			}
			static void debugDarkGreen(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::DarkGreen, text);
			}

			static void debugBlue(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::Blue, text);
			}

			static void debugDarkBlue(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::DarkBlue, text);
			}

			static void debugMagenta(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::Magenta, text);
			}

			static void debugDarkMagenta(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::DarkMagenta, text);
			}


			static void debugYellow(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::Yellow, text);
			}

			static void debugDarkYellow(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::DarkYellow, text);
			}


			static void debugGray(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::Gray, text);
			}

			static void debugDarkGray(...cli::array<Object^>^ text)
			{
				tool::debug(ConsoleColor::DarkGray, text);
			}

			static int debugIndex;

			static String^ GetCallingMethod(StackFrame^ frame)
			{
				auto method = frame->GetMethod();
				String^ type = "";
				String^ name = "";
				if (method != nullptr)
				{
					type = method->DeclaringType->ToString();
					name = method->Name;
				}
				return String::Concat(name, " : ", method, " : ", type);

			}

			static int debugTrace = 3;
			static int LowerTraceRange = 1;

			static void debug(int trace, ...cli::array<Object^>^ text)
			{
				int pdt = debugTrace;
				debugTrace = trace;
				LowerTraceRange = 2;
				debug(text);
				debugTrace = pdt;
			}

			static void debug(...cli::array<String^>^ text)
			{
				String^ s = "";
				String^ calling = "";
				String^ id = "-";
				String^ indent = id;
				if (debugTrace == 0)
					debugTrace = 1;
				//StackFrame[] stacks = new StackFrame[FrameCount];
				for (int si = LowerTraceRange + debugTrace; si >= LowerTraceRange; si--)
				{
					calling = String::Concat(calling, "\n", indent, GetCallingMethod(gcnew StackFrame(si)));
					indent += id;
				}

				// calling = string.Concat(GetCallingMethod(stacks[3]), "\n  ",stacks[2]);

				s = "";
				for each(String^ o in text)
				{
					s = String::Concat(s, o, "\n");
				}

				Console::WriteLine(String::Concat(debugIndex, " : ", s));
				Console::WriteLine(calling);

				// Console.WriteLine();
				// Console.WriteLine();
				debugIndex++;
				LowerTraceRange = 1;
			}
			static void debug(String^ text)
			{
				return;
			}
			
			
			static void debug(...cli::array<Object^>^ text)
			{
				String^ s = "";
				String^ calling = "";
				String^ id = "-";
				String^ indent = id;
				if (debugTrace == 0)
					debugTrace = 1;

				for (int si = LowerTraceRange + debugTrace; si >= LowerTraceRange; si--)
				{
					calling = String::Concat(calling, "\n", indent, GetCallingMethod(gcnew StackFrame(si)));
					indent += id;
				}

				s = "";
				for each(Object^ o in text)
				{
					s = String::Concat(s, o->ToString(), "\n");
				}

				Console::WriteLine(String::Concat(debugIndex, " : ", s));
				Console::WriteLine(calling);
				debugIndex++;
				LowerTraceRange = 1;
			}

			static void show(...cli::array<Object^>^  text)
			{
				StackFrame^ frame = gcnew StackFrame(1);
				auto method = frame->GetMethod();
				auto type = method->DeclaringType;
				auto name = method->Name;
				String^ line = method + "\n" + type + "\n" + name + "\n>>>>>>>>>>>>>>>>>>>>>>>>\n";

				for (int i = 0; i < text->Length; i++)
				{
					if (text[i] != nullptr)
						line = String::Concat(line, "\n", text[i]->ToString());
					else
						line = String::Concat(line, "Value is null");
				}

				MessageBox::Show(line);
			}



			static void show()
			{
				// Tool->Break();
				StackFrame^ frame = gcnew StackFrame(1);
				auto method = frame->GetMethod();
				auto type = method->DeclaringType;
				auto name = method->Name;
				tool::show("Test:\n" + method + "\n" + type + "\n" + name);
			}



			static bool CheckValidString(String^ text)
			{
				if (text == "" || text == nullptr)
					return false;
				else
					return true;
			}

			static bool IsAudioFile(String^ path)
			{
				if (Path::GetExtension(path) == ".wav"
					|| Path::GetExtension(path) == ".mp3"
					|| Path::GetExtension(path) == ".wma"
					|| Path::GetExtension(path) == ".flac")
					return true;
				else
					return false;
			}

			static bool IsTextFile(String^ path)
			{
				if (Path::GetExtension(path) == ".txt"
					|| Path::GetExtension(path) == ".doc"
					|| Path::GetExtension(path) == ".docx")
					return true;
				else
					return false;
			}

			static bool IsPlaylistFile(String^ path)
			{
				if (Path::GetExtension(path) == ".m3u"
					|| Path::GetExtension(path) == ".wpl"
					|| Path::GetExtension(path) == ".pls")
					return true;
				else
					return false;
			}

			static bool IsImageFile(String^ path)
			{
				if (Path::GetExtension(path) == ".jpg"
					|| Path::GetExtension(path) == ".png"
					|| Path::GetExtension(path) == ".bmp")
					return true;
				else
					return false;
			}

			static bool IsVideoFile(String^ path)
			{
				if (Path::GetExtension(path) == ".mp4"
					|| Path::GetExtension(path) == ".mov"
					|| Path::GetExtension(path) == ".avi"
					|| Path::GetExtension(path) == ".wmv"
					|| Path::GetExtension(path) == ".flv"
					|| Path::GetExtension(path) == ".mkv"
					|| Path::GetExtension(path) == ".m4v"
					|| Path::GetExtension(path) == ".3pg"
					|| Path::GetExtension(path) == ".mkv")
					return true;
				else
					return false;
			}

			static bool IsAlphabet(String^ character)
			{
				String^ c = character->ToLower();
				if (c == "a" | c == "b" | c == "c" | c == "d" | c == "e" | c == "f" | c == "f" | c == "g"
					| c == "h" | c == "i" | c == "j" | c == "k" | c == "l" | c == "m" | c == "p" | c == "q"
					| c == "r" | c == "s" | c == "t" | c == "u" | c == "v" | c == "w" | c == "x" | c == "y"
					| c == "z")
				{
					return true;
				}
				return false;
			}

			static List<String^>^ LoadFiles(String^ path, String^ extention)
			{

				cli::array<String^>^ files = Directory::GetFiles(path, "*.*", SearchOption::AllDirectories);
				List<String^>^ fs = gcnew List<String^>();
				for each(String^ f in files)
				{
					String^ ext = Path::GetExtension(f);
					String^ name = Path::GetFileNameWithoutExtension(f);

					if (ext == extention)
					{
						fs->Add(f);
					}
				}

				return fs;
			}
		};
	}
}

