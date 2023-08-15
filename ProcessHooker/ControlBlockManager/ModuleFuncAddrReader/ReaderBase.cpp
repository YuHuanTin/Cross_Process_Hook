

#include "ReaderBase.h"
#include "../../../Utils/Utils.h"

ReaderBase::ReaderBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)) {}

ReaderBase::~ReaderBase() {
    CloseHandle(m_processHandle);
}

std::optional<MODULEENTRY32> ReaderBase::isDllLoaded(const std::string &DllName) const {
    return Utils::RemoteProcess::getProcessModule(m_processID, DllName);
}
