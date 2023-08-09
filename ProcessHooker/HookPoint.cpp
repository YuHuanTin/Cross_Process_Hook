

#include "HookPoint.h"
#include "../Utils/Utils.h"
#include "../Utils/MyException.h"


HookPoint::HookPoint(HANDLE ProcessHandle, DWORD ControlBlockAddr, DWORD HookAddr, std::size_t HookLen, std::unique_ptr<ShellCodeBase> ShellCodeBase)
        : m_processHandle(ProcessHandle),
          m_hookAddr(HookAddr),
          m_hookLen(HookLen) {
    m_shellCodeAddr = (DWORD) Utils::RemoteProcess::allocMemory(m_processHandle);
    auto shellCode = ShellCodeBase->makeShellCode(ControlBlockAddr, m_shellCodeAddr + 0xF00);
    m_shellCodeLen  = shellCode.size();
    Utils::RemoteProcess::writeMemory(m_processHandle, (LPVOID) m_shellCodeAddr, shellCode.data(), shellCode.size());

    m_hookState = ALLOC_MEMORY;
}

DWORD HookPoint::getHookAddr() const noexcept {
    return m_hookAddr;
}

DWORD HookPoint::getShellCodeAddr() const noexcept {
    return m_shellCodeAddr;
}

void HookPoint::coverCodeJmp() {
    if (m_hookState == ALLOC_MEMORY) {
        if (m_hookLen < 5) {
            throw MyException("hook len less than 5 Bytes", __FUNCTION__);
        }

        // 保存被覆盖的代码
        m_coverData = Utils::RemoteProcess::readMemory<uint8_t>(m_processHandle, (DWORD) m_hookAddr, m_hookLen);
        /// 先在 shellcode 结尾添加jmp回去的代码
        std::vector<std::uint8_t> patchCode(5 + m_hookLen);

        for (std::size_t i = 0; i < m_hookLen; ++i) {
            patchCode[i] = *(uint8_t *) (m_coverData.get() + i);
        }

        patchCode[m_hookLen] = 0xE9;                                             // jmp
        DWORD *offset = (DWORD *) (patchCode.data() + m_hookLen + 1);            // jmp-offset
        *offset = m_hookAddr - ((DWORD) m_shellCodeAddr + m_shellCodeLen) - 5;
        Utils::RemoteProcess::writeMemory(m_processHandle, (LPVOID) ((DWORD) m_shellCodeAddr + m_shellCodeLen), patchCode.data(), patchCode.size());
        /// 写跳转到 shellcode 的代码
        patchCode.resize(m_hookLen, 0x90);                         // 全部填充 nop
        patchCode[0] = 0xE9;                                                     // jmp
        offset = (DWORD *) (patchCode.data() + 1);                               // jmp-offset
        *offset = (DWORD) m_shellCodeAddr - m_hookAddr - 5;
        Utils::RemoteProcess::writeMemory(m_processHandle, (LPVOID) m_hookAddr, patchCode.data(), patchCode.size());
    }

    m_hookState = COMMIT_MEMORY;
}

void HookPoint::recoverCodeJmp() {
    if (m_hookState == COMMIT_MEMORY)
        Utils::RemoteProcess::writeMemory(m_processHandle, (LPVOID) m_hookAddr, m_coverData.get(), m_hookLen);

    m_hookState = ALLOC_MEMORY;
}

HookPoint::~HookPoint() {
    recoverCodeJmp();
    Utils::RemoteProcess::freeMemory(m_processHandle, (LPVOID) m_shellCodeAddr);
}




