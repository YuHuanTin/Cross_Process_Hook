

#ifndef CROSS_PROCESS_HOOK_SOCKETHOOK_H
#define CROSS_PROCESS_HOOK_SOCKETHOOK_H

#include "ProcessHookBase.h"
#include <thread>

class SocketHook : public ProcessHookBase {
public:
    explicit SocketHook(const std::string &ProcessName);

    explicit SocketHook(DWORD ProcessID);

    void addHook(DWORD HookAddr, std::size_t HookLen) override;

    bool commitHook(std::function<bool(DataBuffer *)> FuncRecvData) override;

    bool deleteHook(DWORD HookAddress) override;

private:

    std::thread m_socketRecvThread;
};


#endif //CROSS_PROCESS_HOOK_SOCKETHOOK_H
