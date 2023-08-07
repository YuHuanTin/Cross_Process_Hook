

#include "ProcessHookBase.h"
#include "../Utils/Utils.h"

ProcessHookBase::ProcessHookBase(const std::string &ProcessName) : ProcessHookBase(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

ProcessHookBase::ProcessHookBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)) {}

ProcessHookBase::~ProcessHookBase() {
    CloseHandle(m_processHandle);
}



