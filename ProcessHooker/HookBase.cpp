

#include "HookBase.h"
#include "../Utils/Utils.h"


HookBase::HookBase(const std::string &ProcessName) : HookBase(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

HookBase::HookBase(DWORD ProcessID)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)) {}

HookBase::~HookBase() {
    CloseHandle(m_processHandle);
}
