

#ifndef CROSS_PROCESS_HOOK_HOOKPOINT_H
#define CROSS_PROCESS_HOOK_HOOKPOINT_H

#include <string>
#include <memory>
#include <vector>
#include <Windows.h>

#include "../ShellCodeMaker/ShellCodeBase.h"

class HookPoint {
private:
    enum HookState {
        ALLOC_MEMORY, COMMIT_MEMORY
    };
    DWORD                           m_processID;
    HANDLE                          m_processHandle;
    DWORD                           m_hookAddr;                    // hook地址
    std::size_t                     m_hookLen;                     // hook长度（自动填写 nop）
    DWORD                           m_shellCodeAddr;               // shellcode地址
    std::size_t                     m_shellCodeLen;                // shellcode长度
    std::unique_ptr<std::uint8_t[]> m_coverData;                   // 被覆盖的代码
    HookState                       m_hookState;                   // hook状态

public:
    HookPoint(DWORD ProcessID, DWORD ControlBlockAddr, DWORD HookAddr, std::size_t HookLen, std::unique_ptr<ShellCodeBase> ShellCodeBase);

    DWORD getHookAddr() const noexcept;

    DWORD getShellCodeAddr() const noexcept;

    void coverCodeJmp();

    void recoverCodeJmp();

    ~HookPoint();
};


#endif //CROSS_PROCESS_HOOK_HOOKPOINT_H
