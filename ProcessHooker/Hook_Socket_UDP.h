

#ifndef CROSS_PROCESS_HOOK_HOOK_SOCKET_UDP_H
#define CROSS_PROCESS_HOOK_HOOK_SOCKET_UDP_H

#include "HookBase.h"
#include <thread>

class Hook_Socket_UDP : public HookBase {
public:
    explicit Hook_Socket_UDP(const std::string &ProcessName);

    explicit Hook_Socket_UDP(DWORD ProcessID);

    void addHook(std::size_t HookAddr, std::size_t HookLen) override;

    void commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) override;

    void deleteHook(std::size_t HookAddress) override;

    ~Hook_Socket_UDP() override;

private:
    std::thread m_socketRecvThread;
    SOCKET      m_hSocket;
};


#endif //CROSS_PROCESS_HOOK_HOOK_SOCKET_UDP_H
