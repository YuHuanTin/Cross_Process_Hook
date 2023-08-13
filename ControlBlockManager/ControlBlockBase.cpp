
#include "ControlBlockBase.h"

ControlBlockBase::ControlBlockBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_controlBlock(std::make_unique<ControlBlock>()),
          m_rvaReader(std::make_unique<ReadFromFile>(m_processID)) {}

ControlBlockBase::~ControlBlockBase() {
    /// 释放目标进程因为 hook 而加载的 dll
    for (const auto &dllName: m_hookLoadDlls) {
        if (m_rvaReader->isDllLoaded(dllName)) {
            Utils::RemoteProcess::freeDll(m_processHandle, m_freeLibraryAddr, Utils::RemoteProcess::getProcessModule(m_processID, dllName).value().hModule);
        }
    }

    CloseHandle(m_processHandle);
}

void ControlBlockBase::injectControlBlock() {
    m_controlAddr = Utils::RemoteProcess::allocMemory(m_processHandle);
    Utils::RemoteProcess::writeMemory(m_processHandle, m_controlAddr->getAddr(), m_controlBlock.get(), sizeof(ControlBlock));
}

LPVOID ControlBlockBase::getControlBlockRemoteAddr() const noexcept {
    return m_controlAddr->getAddr();
}

std::unique_ptr<ControlBlock> &ControlBlockBase::getControlBlockRef() noexcept {
    return m_controlBlock;
}

