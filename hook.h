#pragma once
#include <Windows.h>
class vmthook {
public:
	vmthook(void* p_class, DWORD index, DWORD* replacer);
	~vmthook();
public:
	DWORD* m_pOriginal;
private:
	DWORD** m_pVMTFunctionPointer;
};

class callhook {
public:
	callhook(PBYTE src, PBYTE callee, UINT bytesToFix, PCHAR outBytes);
	~callhook();
public:
	// function pointer to new memory filled with saved bytes
	LPVOID lpNewFunctionAddress;
private:
	CHAR ogBytes[16];
	PBYTE svSrc;
	UINT svBytesToFix;
};