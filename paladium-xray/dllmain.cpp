#include <Windows.h>
#include "IlIllllIIIlIlIlIlIlIlIlIlIlII.hpp"

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call != DLL_PROCESS_ATTACH) {
        return TRUE;
    }

    DisableThreadLibraryCalls(hModule);

    HANDLE thread_handle = CreateThread(0, 0, [](LPVOID hMod) -> DWORD {
        IlIllllIIIlIlIlIlIlIlIlIlIlII::initialize(static_cast<HMODULE>(hMod));
        return 0;
        }, hModule, 0, 0);

    if (thread_handle)
    {
        CloseHandle(thread_handle);
    }

    return TRUE;
}
