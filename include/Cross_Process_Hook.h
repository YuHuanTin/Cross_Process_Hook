#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H

#include <string>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <WinSock2.h>
#include <Windows.h>
#include <TlHelp32.h>

#include "ControlBlockManager.h"

#pragma comment(lib,"ws2_32.lib")

class ProcessHook {
private:
    enum HookMethod {
        Socket, Pipe, Shared_Memory
    };

    DWORD                            m_processID;
    HANDLE                           m_processHandle;
    HookMethod                       m_hookMethod;
    std::unordered_map<DWORD, DWORD> m_hooks;

    std::unique_ptr<ControlBlockManager> m_controlBlockManager;
public:
    explicit ProcessHook(const std::string &ProcessName, HookMethod HookMethod = HookMethod::Socket);

    explicit ProcessHook(DWORD ProcessID, HookMethod HookMethod = HookMethod::Socket);

    /**
     * HookLen 至少为 5 字节
     *
     * @param HookAddress
     * @param HookLen
     * @return
     */
    bool AddHook(DWORD HookAddress, unsigned HookLen = 5);

    bool CommitHook();

    bool DeleteHook(DWORD HookAddress);

    ~ProcessHook();
};

#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H