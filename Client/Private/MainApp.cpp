#include "pch.h"
#include "MainApp.h"

BEGIN(Client)

void CMainApp::Run()
{
	Server::Print_HelloWorld();
}

shared_ptr<CMainApp> CMainApp::Create()
{
	return make_shared<CMainApp>();
}

END