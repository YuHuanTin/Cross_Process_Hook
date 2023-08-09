
#include "ControlBlockBase.h"
#include "../Utils/Utils.h"

ControlBlockBase::ControlBlockBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_controlBlock(std::make_unique<ControlBlock>()),
          m_rvaReader(std::make_unique<ReadFromFile>(m_processID)) {}

ControlBlockBase::~ControlBlockBase() {
    /// �ͷ�Ŀ�������Ϊ hook �����ص� dll
    for (const auto &one: m_hookLoadDlls) {
        if (m_rvaReader->isDllLoaded(one)) {
            Utils::RemoteProcess::freeDll(m_processHandle, m_freeLibraryAddr, Utils::RemoteProcess::getProcessModule(m_processID, one).value().hModule);
        }
    }

    /// �ͷ�Ŀ����̵Ŀ��ƿ��ڴ�
    if (m_controlBlockRemoteAddr) {
        Utils::RemoteProcess::freeMemory(m_processHandle, m_controlBlockRemoteAddr);
    }
    CloseHandle(m_processHandle);
}

void ControlBlockBase::injectControlBlock() {
    m_controlBlockRemoteAddr = Utils::RemoteProcess::allocMemory(m_processHandle);
    Utils::RemoteProcess::writeMemory(m_processHandle, m_controlBlockRemoteAddr, m_controlBlock.get(), sizeof(ControlBlock));
}

DWORD ControlBlockBase::getControlBlockRemoteAddr() const noexcept {
    return (DWORD)m_controlBlockRemoteAddr;
}

std::unique_ptr<ControlBlock> &ControlBlockBase::getControlBlockRef() noexcept {
    return m_controlBlock;
}

