#ifndef CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#define CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H
#include <string>
#include <iostream>
#include "ShellCodeMaker.h"

#pragma comment(lib,"Ws2_32.lib")

class ProcessHook{
private:
    ProcessInformationT processInformation;
    std::vector<HookT> vHooks;
public:
    ProcessHook(const std::string &ProcessName, HookMethodE HookMethod, fn_cb CallBack);
    ProcessHook(DWORD ProcessID, HookMethodE HookMethod, fn_cb CallBack);

    /// \param HookAddress hook的起始地址
    /// \param HookLen hook占用的字节
    /// \return
    bool CtorHook(DWORD HookAddress, unsigned HookLen);

    bool CommitMem();

    bool DeleteHook(DWORD HookAddress);
};

#endif //CROSS_PROCESS_HOOK_CROSS_PROCESS_HOOK_H