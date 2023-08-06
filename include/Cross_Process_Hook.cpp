
#include "Cross_Process_Hook.h"
#include "ShellCodeMaker.h"
#include "../Utils/Utils.h"


ProcessHook::ProcessHook(const std::string &ProcessName, HookMethod HookMethod)
        : ProcessHook(Utils::RemoteProcess::getProcessID(ProcessName).value(), HookMethod) {}

ProcessHook::ProcessHook(DWORD ProcessID, HookMethod HookMethod)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_hookMethod(HookMethod) {

    m_controlBlockManager = std::make_unique<ControlBlockManager>(m_processID);
    if (HookMethod == HookMethod::Socket) {
        m_controlBlockManager->fillSocketArgs();
    }
    m_controlBlockManager->injectControlBlock(m_processHandle);
}

bool ProcessHook::AddHook(DWORD HookAddress, unsigned int HookLen) {
    if (m_hookMethod == HookMethod::Socket) {
        ShellCodeMaker maker;

        auto data = maker.makeSocketShellCode((DWORD) m_controlBlockManager->getRemoteControlBlockAddr());

        LPVOID hookAddress = Utils::RemoteProcess::allocMemory(m_processHandle);
        Utils::RemoteProcess::writeMemory(m_processHandle, hookAddress, data.data(), data.size());

        printf("%d\n", (int) hookAddress);
    }

    return true;
}

bool ProcessHook::CommitHook() {
    return false;
}

bool ProcessHook::DeleteHook(DWORD HookAddress) {
    return false;
}

ProcessHook::~ProcessHook() {
    CloseHandle(m_processHandle);
}

