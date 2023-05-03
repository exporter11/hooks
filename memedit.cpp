#include "memedit.h"
void EditMemory(PCHAR src, PCHAR opcode, UINT opcodeLen, UINT bytesToFix) {
    UINT i = 0;

    //edit
    for (; i < opcodeLen; i++) {
        src[i] = opcode[i % opcodeLen];
    }

    if (!bytesToFix)
        return;

    for (i = opcodeLen; i < opcodeLen + bytesToFix; i++) {
        src[i] = '\x90';
    }
}

void EditMemory(PCHAR src, PCHAR opcode, UINT opcodeLen, UINT bytesToFix, PCHAR outOpCode) {
    if (!outOpCode) {
        EditMemory(src, opcode, opcodeLen, bytesToFix);
        return;
    }

    UINT i = 0;

    //edit
    for (; i < opcodeLen; i++) {
        outOpCode[i] = src[i];
        src[i] = opcode[i % opcodeLen];
    }

    if (!bytesToFix)
        return;

    for (i = opcodeLen; i < opcodeLen + bytesToFix; i++) {
        outOpCode[i] = src[i];
        src[i] = '\x90';
    }
}

// -1 = 10000000 00000000 00000000 00000001
// -1(s) = 2147483649(u)

void EditProtectedMemory(PCHAR src, PCHAR opcode, UINT opcodeLen, UINT bytesToFix, PCHAR outOpCode) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());

    DWORD op;
    VirtualProtectEx(hProcess, src, opcodeLen + bytesToFix, PAGE_EXECUTE_READWRITE, &op);

    if (!outOpCode) {
        EditMemory(src, opcode, opcodeLen, bytesToFix);
    }
    else {
        EditMemory(src, opcode, opcodeLen, bytesToFix, outOpCode);
    }

    VirtualProtectEx(hProcess, src, opcodeLen + bytesToFix, op, &op);
    CloseHandle(hProcess);
}