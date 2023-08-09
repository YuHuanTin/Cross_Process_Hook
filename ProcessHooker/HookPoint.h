

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
    DWORD                           m_hookAddr;                    // hook��ַ
    std::size_t                     m_hookLen;                     // hook���ȣ��Զ���д nop��
    DWORD                           m_shellCodeAddr;               // shellcode��ַ
    std::size_t                     m_shellCodeLen;                // shellcode����
    std::unique_ptr<std::uint8_t[]> m_coverData;                   // �����ǵĴ���
    HookState                       m_hookState;                   // hook״̬

public:
    HookPoint(DWORD ProcessID, DWORD ControlBlockAddr, DWORD HookAddr, std::size_t HookLen, std::unique_ptr<ShellCodeBase> ShellCodeBase);

    DWORD getHookAddr() const noexcept;

    DWORD getShellCodeAddr() const noexcept;

    void coverCodeJmp();

    void recoverCodeJmp();

    ~HookPoint();
};


#endif //CROSS_PROCESS_HOOK_HOOKPOINT_H
