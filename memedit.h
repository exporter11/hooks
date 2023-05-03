#pragma once
#include <Windows.h>
 
void EditMemory(PCHAR src, PCHAR opcode, UINT opcodeLen, UINT bytesToFix, PCHAR outOpCode);
void EditProtectedMemory(PCHAR src, PCHAR opcode, UINT opcodeLen, UINT bytesToFix, PCHAR outOpCode);
#define NopProtectedMemory(src, len, out) EditProtectedMemory(src, NULL, NULL, len, out)
#define NopMemory(src, len, out) EditMemory(src, NULL, NULL, len, out)