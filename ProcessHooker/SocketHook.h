

#ifndef CROSS_PROCESS_HOOK_SOCKETHOOK_H
#define CROSS_PROCESS_HOOK_SOCKETHOOK_H

#include "ProcessHookBase.h"
#include <thread>

class SocketHook : public ProcessHookBase {
public:
    explicit SocketHook(const std::string &ProcessName);

    explicit SocketHook(DWORD ProcessID);

    void addHook(DWORD HookAddr, std::size_t HookLen) override;

    void commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) override;

    void deleteHook(DWORD HookAddress) override;

    ~SocketHook();

private:

    std::thread m_socketRecvThread;
};


#endif //CROSS_PROCESS_HOOK_SOCKETHOOK_H
