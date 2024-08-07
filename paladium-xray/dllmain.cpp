#include <Windows.h>
#include "xray.hpp"

void __stdcall MainRoutine(HMODULE hModule)
{
    Sleep(5000 + (rand() % 5000));
    xray::initialize(hModule);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        HANDLE hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainRoutine, hModule, 0, nullptr);
        if (hThread) {
            CloseHandle(hThread);
        }
    }

    return TRUE;
}
