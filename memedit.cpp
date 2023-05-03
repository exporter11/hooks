#include "memedit.h"
void EditMemory(PCHAR src, PCHAR opCode, UINT opCodeLen, UINT bytesToFix) {
    UINT i = 0;

    //edit
    for (; i < opCodeLen; i++) {
        src[i] = opCode[i % opCodeLen];
    }

    if (!bytesToFix)
        return;

    for (i = opCodeLen; i < opCodeLen + bytesToFix; i++) {
        src[i] = '\x90';
    }
}

void EditMemory(PCHAR src, PCHAR opCode, UINT opCodeLen, UINT bytesToFix, PCHAR outOpCode) {
    if (!outOpCode) {
        EditMemory(src, opCode, opCodeLen, bytesToFix);
        return;
    }

    UINT i = 0;

    //edit
    for (; i < opCodeLen; i++) {
        outOpCode[i] = src[i];
        src[i] = opCode[i % opCodeLen];
    }

    if (!bytesToFix)
        return;

    for (i = opCodeLen; i < opCodeLen + bytesToFix; i++) {
        outOpCode[i] = src[i];
        src[i] = '\x90';
    }
}

// -1 = 10000000 00000000 00000000 00000001
// -1(s) = 2147483649(u)

void EditProtectedMemory(PCHAR src, PCHAR opCode, UINT opCodeLen, UINT bytesToFix, PCHAR outOpCode) {
    const HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());

    DWORD op;
    VirtualProtectEx(hProcess, src, opCodeLen + bytesToFix, PAGE_EXECUTE_READWRITE, &op);

    if (!outOpCode) {
        EditMemory(src, opCode, opCodeLen, bytesToFix);
    }
    else {
        EditMemory(src, opCode, opCodeLen, bytesToFix, outOpCode);
    }

    VirtualProtectEx(hProcess, src, opCodeLen + bytesToFix, op, &op);
    CloseHandle(hProcess);
}