#include "hook.h"
#include "memedit.h"

vmthook::vmthook(void* p_class, DWORD index, DWORD* replacer) {
    m_pVMTFunctionPointer = 0;
	m_pOriginal = 0;

	DWORD** pFunction = ((DWORD***)p_class)[0] + index;

	if (!pFunction)
		return;

	m_pVMTFunctionPointer = pFunction;
	m_pOriginal = *pFunction;

	DWORD op;
	VirtualProtect(pFunction, sizeof(DWORD**), PAGE_EXECUTE_READWRITE, &op);
	*pFunction = replacer;
	VirtualProtect(pFunction, sizeof(DWORD**), op, &op);
}

vmthook::~vmthook() {
	if (!(m_pVMTFunctionPointer && m_pOriginal))
		return;

	DWORD op;
	VirtualProtect(m_pVMTFunctionPointer, sizeof(DWORD*), PAGE_EXECUTE_READWRITE, &op);
	*m_pVMTFunctionPointer = m_pOriginal;
	VirtualProtect(m_pVMTFunctionPointer, sizeof(DWORD*), op, &op);
}

callhook::callhook(PBYTE src, PBYTE callee, UINT bytesToFix, PCHAR outBytes) {
    svSrc = src;
    svBytesToFix = bytesToFix;

    //hook
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());
    DWORD dwRelativeAddress = (DWORD)(callee - src - 5);

    DWORD op;
    VirtualProtectEx(hProcess, src, bytesToFix + 5, PAGE_EXECUTE_READWRITE, &op);

    //save
    UINT i = 0;
    for (; i < 5 + bytesToFix; i++) {
        if (outBytes)
            outBytes[i] = src[i];

        ogBytes[i] = src[i];
    }

    //edit
    *(src) = '\xE8';
    *((DWORD*)(src + 1)) = dwRelativeAddress;

    //fix structure
    if (bytesToFix)
        EditMemory((PCHAR)src + 5, nullptr, NULL, bytesToFix, nullptr);

    VirtualProtectEx(hProcess, src, bytesToFix + 5, op, &op);
    CloseHandle(hProcess);
    if (outBytes) {
        lpNewFunctionAddress = VirtualAlloc(nullptr, 5 + bytesToFix, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!lpNewFunctionAddress)
            return;
        
        memcpy(lpNewFunctionAddress, outBytes, 5+bytesToFix);
    } 
    else
        lpNewFunctionAddress = nullptr;


}

callhook::~callhook() {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());

    DWORD op;
    VirtualProtectEx(hProcess, svSrc, svBytesToFix + 5, PAGE_EXECUTE_READWRITE, &op);

    UINT i = 0;
    for (; i < svBytesToFix + 5; i++) {
        svSrc[i] = ogBytes[i];
    }

    VirtualProtectEx(hProcess, svSrc, svBytesToFix + 5, op, &op);
    CloseHandle(hProcess);

    if (!lpNewFunctionAddress)
        return;

    VirtualFree(lpNewFunctionAddress, NULL, MEM_RELEASE);
}