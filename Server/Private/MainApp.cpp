#include "pch.h"
#include "MainApp.h"

BEGIN(Server)

void CMainApp::Run()
{
	Print_HelloWorld();
}

shared_ptr<CMainApp> CMainApp::Create()
{
	return make_shared<CMainApp>();
}

END