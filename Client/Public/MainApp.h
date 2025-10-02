#pragma once

BEGIN(Client)

class CMainApp final
{
public:
    CMainApp() = default;
    ~CMainApp() = default;

public:
    void Run();

public:
    shared_ptr<CMainApp> static Create();
};

END
