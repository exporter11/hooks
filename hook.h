#pragma once
#include <Windows.h>
class CVmtHook {
public:
	CVmtHook(void* pClass, DWORD index, DWORD* replacer);
	~CVmtHook();
public:
	DWORD* m_pOriginal;
private:
	DWORD** m_pVmtFunctionPointer_;
};

class CCallHook {
public:
	CCallHook(PBYTE src, PBYTE callee, UINT bytesToFix, PCHAR outBytes);
	~CCallHook();
public:
	// function pointer to new memory filled with saved bytes
	LPVOID m_lpNewFunctionAddress;
private:
	CHAR m_pOgBytes_[16]{};
	PBYTE m_pSvSrc_;
	UINT m_uSvBytesToFix_;
};