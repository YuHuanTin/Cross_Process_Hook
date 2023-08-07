

#include "RVAReaderBase.h"
#include "../Utils/Utils.h"

RVAReaderBase::RVAReaderBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)) {}

RVAReaderBase::~RVAReaderBase() {
    CloseHandle(m_processHandle);
}

std::optional<MODULEENTRY32> RVAReaderBase::isDllLoaded(const std::string &DllName) const {
    return Utils::RemoteProcess::getProcessModule(m_processID, DllName);
}
