

#include "ProcessHookBase.h"
#include "../Utils/Utils.h"



ProcessHookBase::ProcessHookBase(const std::string &ProcessName) : ProcessHookBase(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

ProcessHookBase::ProcessHookBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)) {}

ProcessHookBase::~ProcessHookBase() {
    CloseHandle(m_processHandle);
}

void ProcessHookBase::patchCodeJmp() {
    for (auto &one: m_hooks) {
        if (one.hookLen < 5) {
            throw MyException("hook len less than 5 Bytes", __FUNCTION__);
        }
        std::vector<std::uint8_t> patchCode(one.hookLen, 0x90); // 全部填充 nop
        patchCode[0] = 0xE9;                                  // jmp
        auto offset = (DWORD *) (patchCode.data() + 1);    // jmp-offset
        *offset = (DWORD)one.shellCodeAddr - one.hookAddr - 5;

        // 保存被覆盖的代码
        auto coveredCode = Utils::RemoteProcess::readMemory<uint8_t>(m_processHandle, (DWORD) one.hookAddr, one.hookLen);

        // 写入jmp代码
        Utils::RemoteProcess::writeMemory(m_processHandle, (LPVOID) one.hookAddr, patchCode.data(), patchCode.size());

        // 写跳转回去的代码
        patchCode.resize(one.hookLen + 5);
        for (std::size_t i = 0; i < one.hookLen; ++i) {
            patchCode[i] = *(uint8_t *)(coveredCode.get() + i);
        }
        patchCode[one.hookLen] = 0xE9; // jmp
        offset = (DWORD *) (patchCode.data() + one.hookLen + 1); // jmp-offset
        *offset = one.hookAddr - ((DWORD)one.shellCodeAddr + one.shellCodeLen) - 5;

        Utils::RemoteProcess::writeMemory(m_processHandle, (LPVOID) ((DWORD) one.shellCodeAddr + one.shellCodeLen), patchCode.data(), patchCode.size());
    }
}
