

#ifndef CROSS_PROCESS_HOOK_HOOKPOINT_H
#define CROSS_PROCESS_HOOK_HOOKPOINT_H

#include <string>
#include <memory>
#include <vector>
#include <Windows.h>

#include "../Utils/Utils.h"
#include "../ShellCodeMaker/ShellCodeBase.h"

class HookPoint {
private:
    enum HookState {
        ALLOC_MEMORY, COMMIT_MEMORY
    };
    DWORD                                   m_processID;
    HANDLE                                  m_processHandle;
    std::size_t                             m_hookAddr;                    // hook��ַ
    std::size_t                             m_hookLen;                     // hook���ȣ��Զ���д nop��
    std::unique_ptr<AutoDelete_AllocMemory> m_shellCodeAddr;               // shellcode��Դ
    std::size_t                             m_shellCodeLen;                // shellcode����
    std::unique_ptr<std::uint8_t[]>         m_coverData;                   // �����ǵĴ���
    HookState                               m_hookState;                   // hook״̬

public:
    HookPoint(DWORD ProcessID, std::size_t ControlBlockAddr, std::size_t HookAddr, std::size_t HookLen, std::unique_ptr<ShellCodeBase> ShellCodeBase);

    std::size_t getHookAddr() const noexcept;

    void coverCodeJmp();

    void recoverCodeJmp();

    ~HookPoint();
};


#endif //CROSS_PROCESS_HOOK_HOOKPOINT_H
