#include "patternScanner.h"

//all things we gonna be using
template class patternScanner<DWORD*, DWORD*>;

template <class T, typename D> 
UINT patternScanner<T, D>::getPatternLength(const char* pattern) {
    UINT length = 0;
    while (strcmp(&pattern[length], " XEND")) {
        length++;
    }

    return length;
}

template <class T, typename D> 
BOOL patternScanner<T, D>::getMINFO(MODULEINFO* mInfo, const char* moduleName) {
    HMODULE hBase = GetModuleHandle(moduleName);
    if (!hBase) {
        return FALSE;
    }

    DWORD procID = GetCurrentProcessId();
    if (!procID) {
        return FALSE;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 1, procID);
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
patternScanner<T, D>::patternScanner(const char* pattern, const char* moduleName, D* pointer) {
    MODULEINFO mInfo{};
    if (!getMINFO(&mInfo, moduleName))
        return;

    PCHAR pCurByte = (PCHAR)mInfo.lpBaseOfDll;
    UINT equalBytes = 0;

    UINT patternLength = getPatternLength(pattern);

    while (mInfo.SizeOfImage) {
        if (pattern[equalBytes] == ' ') {
            ++equalBytes;
            if (equalBytes == patternLength) {
                if (pointer)
                    *pointer = (D)pCurByte;

                m_tValue = *(T*)pCurByte;
                return;
            }

            PCHAR sv_pCurByte = pCurByte;
            pCurByte += sizeof(T);

            UINT bytesLeft = patternLength - equalBytes;
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
                    *pointer = (D)sv_pCurByte;

                m_tValue = *(T*)sv_pCurByte;
                return;
            }

            pCurByte = sv_pCurByte;
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