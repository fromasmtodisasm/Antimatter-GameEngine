#include "Engine.h"

using namespace Antimatter;
using namespace Antimatter::Games;
using namespace System::Diagnostics;

//http://trevinliberty.com/deferred-shading/


void Engine::Construction()
{
	
	scenes = gcnew List<Scene^>();
	fixedUpdateStarted = false;
}

void Antimatter::Games::Engine::InternalLoad()
{
	defaultCamera = gcnew Camera();
	renderer = Render::current;
	Render::current->gxRenderer->render2d.Start();
	Keyboard::defaultKeyboard = gcnew Keyboard();
	keyboard = Keyboard::defaultKeyboard;
	keyboard->HookEvents();
	controllers = gcnew List<Controller^>();
	Mouse::Default = gcnew Mouse();
	Antimatter::Games::EditorMouse::Default = gcnew Antimatter::Games::EditorMouse();
	EditorMouse = EditorMouse::Default;
	Core::HookEvents(Core::CoreEvents);
}

Engine::Engine()
{
	name = "Game";
	Construction();
}

Engine::Engine(String^ Name)
{
	name = Name;
	Construction();
}

void Engine::InitializeGLEW()
{
	glx::Core::DefaultCore->InitializeGLEW();
}

void Engine::CreateAndRun()
{
	RunFixedUpdate();
}


void Engine::RunFixedUpdate()
{
	InternalLoad();
	Load();
	LoadComponents();
	UInt64 ticks_per_second = 25;
	UInt64 skip_ticks = 1000 / ticks_per_second;
	UInt64 max_frameskip = 5;
	UInt64 next_game_tick = (UInt64)GetTickCount();
	int loops;
	float interpolation;

	LARGE_INTEGER perforamceFreq;
	QueryPerformanceFrequency(&perforamceFreq);
	Int64 perfFreq = perforamceFreq.QuadPart;

	UInt64 lastCycleCount = (unsigned long long)_rdtsc;

	LARGE_INTEGER lastCounter;
	QueryPerformanceCounter(&lastCounter);

	while (!glfwWindowShouldClose(renderer->gxCore->glfwWindow))
	{
		loops = 0;
		UInt64 time = GetTickCount();

		while (time> next_game_tick && loops < max_frameskip)
		{
			glfwPollEvents();
			//glfwWaitEvents();
			Update(Core::DeltaTime);
			next_game_tick += skip_ticks;
			loops++;
		}
		
		interpolation = float(time + skip_ticks - next_game_tick)
			/ (float)skip_ticks;

		SwapBuffers();
		UInt64 endCycleCount = (unsigned long long)_rdtsc;
		LARGE_INTEGER endCounter;
		QueryPerformanceCounter(&endCounter);

		Int64 elapsedCounter = endCounter.QuadPart - lastCounter.QuadPart;
		Int64 elapsedCycles = endCycleCount - lastCycleCount;
		Int64 milliSecondsElapsed = 1000*elapsedCounter / perfFreq;
		Int64 fps = perfFreq / elapsedCounter;
		
		FPS = (float)fps;
		Core::DeltaTime = (float)elapsedCounter;
		if (Core::DeltaTime > 0.15f)
			Core::DeltaTime = 0.15f;

		lastCounter = endCounter;
		lastCycleCount = endCycleCount;
	}
	Close();
}

void Engine::Update(float delta)
{
	//Core::DeltaTime = delta;
	//physics->SimulationStep(1 / 60.0f, 10, 100);
	//econtroller.CameraControl(camera);
}
