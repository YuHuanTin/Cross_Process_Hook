

#ifndef CROSS_PROCESS_HOOK_SOCKETHOOK_H
#define CROSS_PROCESS_HOOK_SOCKETHOOK_H

#include "ProcessHookBase.h"

class SocketHook : public ProcessHookBase {
public:
    explicit SocketHook(const std::string &ProcessName);

    explicit SocketHook(DWORD ProcessID);

    bool addHook() override;

    bool commitHook() override;

    bool deleteHook(DWORD HookAddress) override;

private:

};


#endif //CROSS_PROCESS_HOOK_SOCKETHOOK_H
