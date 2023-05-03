#include "patternScanner.h"
#include <Windows.h>
#include <Psapi.h>

//all things we gonna be using
template class CPatternScanner<DWORD*, DWORD*>;


UINT GetPatternLength(const char* pattern) {
    UINT length = 0;
    while (strcmp(&pattern[length], " XEND")) {
        length++;
    }

    return length;
}

BOOL SetupModuleInformationClass(MODULEINFO* mInfo, const char* moduleName) {
    CONST HMODULE hBase = GetModuleHandle(moduleName);
    if (!hBase) {
        return FALSE;
    }

    CONST DWORD procId = GetCurrentProcessId();
    if (!procId) {
        return FALSE;
    }

    CONST HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, procId);
    if (!hProcess) { 
        return FALSE;
    }

    if (!GetModuleInformation(hProcess, hBase, mInfo, sizeof(MODULEINFO))) {
        CloseHandle(hProcess);
        return FALSE;
    }

    CloseHandle(hProcess);
    return TRUE;
}

template <class T, typename D>
CPatternScanner<T, D>::CPatternScanner(const char* pattern, const char* moduleName, D* pointer) {
    MODULEINFO mInfo{};
    if (!SetupModuleInformationClass(&mInfo, moduleName))
        return;

    PCHAR pCurByte = (PCHAR)mInfo.lpBaseOfDll;
    UINT equalBytes = 0;

    CONST UINT patternLength = GetPatternLength(pattern);

    while (mInfo.SizeOfImage) {
        if (pattern[equalBytes] == ' ') {
            ++equalBytes;
            if (equalBytes == patternLength) {
                if (pointer)
                    *pointer = (D)pCurByte;

                m_tValue = *(T*)pCurByte;
                return;
            }

            PCHAR pCurByteSaved = pCurByte;
            pCurByte += sizeof(T);

            CONST UINT bytesLeft = patternLength - equalBytes;
            UINT equalBytesLeft = 0;

            while (equalBytesLeft < bytesLeft) {
                if (*pCurByte != pattern[equalBytes])
                    break;
                
                ++equalBytes;
                ++pCurByte;
                ++equalBytesLeft;
            }

            if (equalBytesLeft == bytesLeft) {
                if (pointer)
                    *pointer = (D)pCurByteSaved;

                m_tValue = *(T*)pCurByteSaved;
                return;
            }

            pCurByte = pCurByteSaved;
            equalBytes = 0;
        }

        if (*pCurByte == pattern[equalBytes])
            ++equalBytes;
        else
            equalBytes = 0;

        --mInfo.SizeOfImage;
        ++pCurByte;
    }
}