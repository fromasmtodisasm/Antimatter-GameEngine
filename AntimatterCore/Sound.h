#pragma once
#include "SystemHeader.h" //<- prevent win.h/form conflicts
#include "GlaxionHeader.h"
//#include <mmsystem.h>
//#include <mciapi.h>
//#pragma comment(lib, "winmm.lib")

//fmod audio engine
//https://codyclaborn.me/tutorial/making-a-basic-fmod-audio-engine-in-c.html

//http://www.un4seen.com/
//http://bass.radio42.com/help/
//another way of playing sounds
//https://stackoverflow.com/questions/22253074/how-to-play-or-open-mp3-or-wav-sound-file-in-c-program/31975785

//https://stackoverflow.com/questions/41908827/how-to-change-volume-of-windows
//other volume control
//https://msdn.microsoft.com/en-us/library/windows/desktop/dd370839(v=vs.85).aspx

//signal processing
//https://gamedev.stackexchange.com/questions/14642/how-do-audio-based-games-such-as-audiosurf-and-beat-hazard-work
//http://www.badlogicgames.com/wordpress/?cat=18
//lib for adjusting sound pitch
//https://www.surina.net/soundtouch/

//melody generation c#
//https://www.codeproject.com/Articles/146628/Steganography-Generate-Melody-from-Text

#pragma comment(lib, "lib/irrKlang.lib")
using namespace System::Media;
using namespace IrrKlang;


//https://www.ambiera.com/irrklang/tutorial-sound-effects-csharp.html
//audio filtering
//https://github.com/zsiciarz/aquila
//https://stackoverflow.com/questions/6938634/any-opencv-like-c-c-library-for-audio-processing

namespace Antimatter {
	namespace Games {

		public ref class Sound
		{
		public:

			//need proper global getter
			static ISoundEngine^ engine = gcnew ISoundEngine();
			String^ path;
			ISound^ clip;
			float volume;
			bool mute;
			static float GlobalVolume = 1.f;

			Sound(String^ File_Path)
			{
				SetFromFile(File_Path);
				volume = 1.f;
				mute = false;
			}

			float ComputeVolume()
			{
				if (!mute)
					return volume*GlobalVolume;
				else
					return 0.f;
			}

			static float dBToVolume(float dB)
			{
				return powf(10.f, 0.05f*dB);
			}

			static float VolumeTodB(float volume)
			{
				return 20.f*log10f(volume);
			}

			void Play()
			{
				//engine->SoundVolume = 0.f;
				ISound^ clip = engine->Play2D(path, false);
				if (clip == nullptr)
				{
					tool::debugError(2,"error, clip is null");
					return;
				}
				clip->Volume = ComputeVolume();
			}

			void Play(float VolumeLevel)
			{
				ISound^ clip = engine->Play2D(path, false);
				if (clip == nullptr)
				{
					tool::debugError(2, "error, clip is null");
					return;
				}
				volume = VolumeLevel;
				clip->Volume = ComputeVolume();
			}

			void PlayLoop()
			{
				ISound^ clip = engine->Play2D(path, true);
				if (clip == nullptr)
				{
					tool::debugError(2,"error, clip is null");
					return;
				}
				;
				clip->Volume = ComputeVolume();
			}

			//please store the sound clip instead of creating a new one each frame
			void PlayLoop(float VolumeLevel)
			{
				if (!File::Exists(path))
				{
					tool::debugError(2, "Could not find file "+path);
					return;
				}
				ISound^ clip = engine->Play2D(path, true,true);
				if (clip == nullptr)
				{
					tool::debugError(2, "error, clip is null");
					return;
				}
				if (!mute)
					clip->Volume = VolumeLevel;
				else
					clip->Volume = 0.0f;
				clip->Paused = false;
				//clip->Volume = ComputeVolume();
			}

			void SetFromFile(String^ file_path)
			{
				path = file_path;
				//IntPtr p = Marshal::StringToHGlobalAnsi(file_path);
				//nativePath = static_cast<wchar_t const *>(p.ToPointer());
			}
		};
	}
}