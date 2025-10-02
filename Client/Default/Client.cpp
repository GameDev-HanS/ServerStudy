#include "pch.h"
#include "MainApp.h"

using namespace Client;

int main()
{
	shared_ptr<CMainApp> mainApp = CMainApp::Create();
	mainApp->Run();

	return 0;
}