#include "hook.h"
#include "memedit.h"

CVmtHook::CVmtHook(void* pClass, DWORD index, DWORD* replacer) {
    m_pVmtFunctionPointer_ = nullptr;
	m_pOriginal = nullptr;

	DWORD** pFunction = ((DWORD***)pClass)[0] + index;

	if (!pFunction)
		return;

    m_pVmtFunctionPointer_ = pFunction;
	m_pOriginal = *pFunction;

	DWORD op;
	VirtualProtect(pFunction, sizeof(DWORD**), PAGE_EXECUTE_READWRITE, &op);
	*pFunction = replacer;
	VirtualProtect(pFunction, sizeof(DWORD**), op, &op);
}

CVmtHook::~CVmtHook() {
	if (!(m_pVmtFunctionPointer_ && m_pOriginal))
		return;

	DWORD op;
	VirtualProtect(m_pVmtFunctionPointer_, sizeof(DWORD*), PAGE_EXECUTE_READWRITE, &op);
	*m_pVmtFunctionPointer_ = m_pOriginal;
	VirtualProtect(m_pVmtFunctionPointer_, sizeof(DWORD*), op, &op);
}

CCallHook::CCallHook(PBYTE src, PBYTE callee, UINT bytesToFix, PCHAR outBytes) {
    m_pSvSrc_ = src;
    m_uSvBytesToFix_ = bytesToFix;

    //hook
    CONST HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());
    const auto dwRelativeAddress = (DWORD)(callee - src - 5);

    DWORD op;
    VirtualProtectEx(hProcess, src, bytesToFix + 5, PAGE_EXECUTE_READWRITE, &op);

    //save
    for (UINT i = 0; i < 5 + bytesToFix; i++) {
        if (outBytes)
            outBytes[i] = src[i];

        m_pOgBytes_[i] = src[i];
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
        m_lpNewFunctionAddress = VirtualAlloc(nullptr, 5 + bytesToFix, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!m_lpNewFunctionAddress)
            return;
        
        memcpy(m_lpNewFunctionAddress, outBytes, 5+bytesToFix);
    } 
    else
        m_lpNewFunctionAddress = nullptr;


}

CCallHook::~CCallHook() {
    CONST HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId());

    DWORD op;
    VirtualProtectEx(hProcess, m_pSvSrc_, m_uSvBytesToFix_ + 5, PAGE_EXECUTE_READWRITE, &op);
    
    for (UINT i = 0; i < m_uSvBytesToFix_ + 5; i++)
        m_pSvSrc_[i] = m_pOgBytes_[i];

    VirtualProtectEx(hProcess, m_pSvSrc_, m_uSvBytesToFix_ + 5, op, &op);
    CloseHandle(hProcess);

    if (!m_lpNewFunctionAddress)
        return;

    VirtualFree(m_lpNewFunctionAddress, NULL, MEM_RELEASE);
}