#pragma once
#include <Windows.h>
 
void EditMemory(PCHAR src, PCHAR opCode, UINT opCodeLen, UINT bytesToFix, PCHAR outOpCode);
void EditProtectedMemory(PCHAR src, const PCHAR opCode, UINT opCodeLen, UINT bytesToFix, PCHAR outOpCode);
#define NOP_PROTECTED_MEMORY(src, len, out) EditProtectedMemory(src, NULL, NULL, len, out)
#define NOP_MEMORY(src, len, out) EditMemory(src, NULL, NULL, len, out)