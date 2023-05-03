//this file may contain errors as i didnt update it i only updated patternscanner.cpp and hook.cpp (minor changes to this one on github only)
#include <Windows.h>
#include <iostream>
#include "patternScanner.h"
#include "memedit.h"
#include "hook.h"

void __stdcall thsFnc() {
    std::cout << "SERUS MAJO\n";
    Sleep(2000);
    return;
}


void threadmain() {
    DWORD* pointer{};
    CPatternScanner<DWORD*, DWORD*> mainSig("\xB8\x05\x00\x00\x00 \x00\xB8\x41\x00\x00\x00 XEND", "memoryedit_Release_Win32.exe", &pointer); //mov eax 05  .. 00 mov eax 41

    std::cout << pointer << '\n';
    if (!pointer)
        return;

    CHAR saved[11]{};
    saved[10] = '\xC3';

    CCallhook callHook((PBYTE)pointer, (PBYTE)thsFnc, 5, saved);

    //call the code we overwrote // NEED TO DO SOMETHING BETTER SO THE FUNCTION WE CALL WILL BE ABLE TO DO THIS ALONE and also free memory at unhook
    void(*fn)() = (void(*)())callHook.m_lpNewFunctionAddress;

    if (!fn)
        return;

    memcpy(fn, saved, 11);
    fn();


    //just test if eax is 0x41 (fn succeeded)
    int b = 0;
    __asm {
        mov b, eax
    }

    std::cout << b << '\n';
    while (!GetAsyncKeyState(VK_END));

    callHook.~CCallHook();


    return;
}
void newThread(HMODULE hModule) {
    threadmain();
    FreeLibraryAndExitThread(hModule, DLL_THREAD_DETACH);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)newThread, hModule, 0, 0);
        if (hThread)
            CloseHandle(hThread);
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
