

#include "HookPoint.h"
#include "../Utils/MyException.h"


HookPoint::HookPoint(DWORD ProcessID, std::size_t ControlBlockAddr, std::size_t HookAddr, std::size_t HookLen, std::unique_ptr<ShellCodeBase> ShellCodeBase)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_hookAddr(HookAddr),
          m_hookLen(HookLen) {
    m_shellCodeAddr = Utils::RemoteProcess::allocMemory(m_processHandle);
    auto shellCode = ShellCodeBase->makeShellCode(ControlBlockAddr, m_shellCodeAddr->getAddr() + 0xF00);
    m_shellCodeLen = shellCode.size();
    Utils::RemoteProcess::writeMemory(m_processHandle, m_shellCodeAddr->getAddr(), shellCode.data(), shellCode.size());

    m_hookState = ALLOC_MEMORY;
}

std::size_t HookPoint::getHookAddr() const noexcept {
    return m_hookAddr;
}

void HookPoint::coverCodeJmp() {
    if (m_hookState == ALLOC_MEMORY) {
        if (m_hookLen < 5) {
            throw MyException("hook len less than 5 Bytes", __FUNCTION__);
        }

        /// 保存被覆盖的代码
        m_coverData = Utils::RemoteProcess::readMemory<uint8_t>(m_processHandle, m_hookAddr, m_hookLen);

        /// 先在 shellcode 结尾添加jmp回去的代码
        std::vector<std::uint8_t> patchCode(5 + m_hookLen);

        for (std::size_t i = 0; i < m_hookLen; ++i) {
            patchCode[i] = *(uint8_t *) (m_coverData.get() + i);
        }

        patchCode[m_hookLen] = 0xE9;                                                    // jmp
        std::size_t *offset = (std::size_t *) (patchCode.data() + m_hookLen + 1);       // jmp-offset
        *offset = m_hookAddr - (m_shellCodeAddr->getAddr() + m_shellCodeLen) - 5;
        Utils::RemoteProcess::writeMemory(m_processHandle, m_shellCodeAddr->getAddr() + m_shellCodeLen, patchCode.data(), patchCode.size());

        /// 写跳转到 shellcode 的代码
        patchCode.resize(m_hookLen, 0x90);                                // 全部填充 nop
        patchCode[0] = 0xE9;                                                            // jmp
        offset = (std::size_t *) (patchCode.data() + 1);                                // jmp-offset
        *offset = m_shellCodeAddr->getAddr() - m_hookAddr - 5;
        Utils::RemoteProcess::writeMemory(m_processHandle, m_hookAddr, patchCode.data(), patchCode.size());
    }

    m_hookState = COMMIT_MEMORY;
}

void HookPoint::recoverCodeJmp() {
    if (m_hookState == COMMIT_MEMORY)
        Utils::RemoteProcess::writeMemory(m_processHandle, m_hookAddr, m_coverData.get(), m_hookLen);

    m_hookState = ALLOC_MEMORY;
}

HookPoint::~HookPoint() {
    recoverCodeJmp();
    CloseHandle(m_processHandle);
}




