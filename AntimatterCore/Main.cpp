#include "SystemHeader.h"
#include "GlaxionHeader.h"
#include "TestGame.h"

using namespace Antimatter::Games;

int main(cli::array<System::String^>^ args)
{
	TestGame^ tg = gcnew TestGame();
	tg->CreateAndRun();
}

