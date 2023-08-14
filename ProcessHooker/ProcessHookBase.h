

#ifndef CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H
#define CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H

#include <memory>
#include <vector>
#include <string>
#include <Windows.h>
#include <functional>

#include "HookPoint.h"
#include "../ControlBlockManager/ControlBlockBase.h"

class ProcessHookBase {
public:
    explicit ProcessHookBase(const std::string &ProcessName);

    explicit ProcessHookBase(DWORD ProcessID);

    virtual void addHook(std::size_t HookAddr, std::size_t HookLen) = 0;

    virtual void commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) = 0;

    virtual void deleteHook(std::size_t HookAddress) = 0;

    virtual ~ProcessHookBase();

protected:
    DWORD  m_processID;
    HANDLE m_processHandle;

    std::unique_ptr<ControlBlockBase>       m_controlBlock;        // ¿ØÖÆ¿é×ÊÔ´
    std::vector<std::unique_ptr<HookPoint>> m_hooks;
};


#endif //CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H
