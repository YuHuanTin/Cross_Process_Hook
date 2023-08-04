#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H

#include <string>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <Windows.h>
#include <TlHelp32.h>


class ProcessHook {
private:
    enum HookMethod {
        Socket, Pipe, Shared_Memory
    };


    DWORD                            m_processID;
    HookMethod                       m_hookMethod;
    std::unordered_map<DWORD, DWORD> m_hooks;

public:
    explicit ProcessHook(const std::string &ProcessName, HookMethod HookMethod = HookMethod::Socket);

    explicit ProcessHook(DWORD ProcessID, HookMethod HookMethod = HookMethod::Socket);

    bool AddHook(DWORD HookAddress, unsigned HookLen);

    bool CommitHook();

    bool DeleteHook(DWORD HookAddress);
};

#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H