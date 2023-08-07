

#ifndef CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H
#define CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H

#include <memory>
#include <vector>
#include <string>
#include <Windows.h>

#include "../ControlBlockManager/ControlBlockBase.h"

class ProcessHookBase {
public:
    explicit ProcessHookBase(const std::string &ProcessName);

    explicit ProcessHookBase(DWORD ProcessID);

    virtual bool addHook() = 0;

    virtual bool commitHook() = 0;

    virtual bool deleteHook(DWORD HookAddress) = 0;

    virtual ~ProcessHookBase();

protected:
    DWORD  m_processID;
    HANDLE m_processHandle;

    std::unique_ptr<ControlBlockBase> m_controlBlock; // 控制块资源
    std::vector<std::uint32_t>        m_hooksAddr;    // 目标进程 hook 表
};


#endif //CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H
