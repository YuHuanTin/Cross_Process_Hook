

#ifndef CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H
#define CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H

#include <memory>
#include <vector>
#include <string>
#include <Windows.h>
#include <functional>

#include "../ControlBlockManager/ControlBlockBase.h"

class ProcessHookBase {
public:
    explicit ProcessHookBase(const std::string &ProcessName);

    explicit ProcessHookBase(DWORD ProcessID);

    virtual void addHook(DWORD HookAddr, std::size_t HookLen) = 0;

    virtual void commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) = 0;

    virtual bool deleteHook(DWORD HookAddress) = 0;

    virtual ~ProcessHookBase();

protected:
    void patchCodeJmp();

    DWORD  m_processID;
    HANDLE m_processHandle;

    std::unique_ptr<ControlBlockBase> m_controlBlock;        // 控制块资源

    struct HookAddrLenWithShellCodeAddr {
        DWORD       hookAddr;                    // hook地址
        std::size_t hookLen;                     // hook长度（自动填写 nop）
        LPVOID      shellCodeAddr;               // shellcode地址
        std::size_t shellCodeLen;                // shellcode长度
        HookAddrLenWithShellCodeAddr(DWORD HookAddr, std::size_t HookLen, LPVOID ShellCodeAddr, std::size_t ShellCodeLen) noexcept
                : hookAddr(HookAddr),
                  hookLen(HookLen),
                  shellCodeAddr(ShellCodeAddr),
                  shellCodeLen(ShellCodeLen) {}
    };

    std::vector<HookAddrLenWithShellCodeAddr> m_hooks;
};


#endif //CROSS_PROCESS_HOOK_PROCESSHOOKBASE_H
