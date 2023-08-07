

#ifndef CROSS_PROCESS_HOOK_SOCKETSHELLCODEX86_H
#define CROSS_PROCESS_HOOK_SOCKETSHELLCODEX86_H

#include "ShellCodeBase.h"

class SocketShellCodeX86 : public ShellCodeBase {
public:

    std::vector<std::uint8_t> makeShellCode(DWORD ControlBlockAddr, DWORD RegisterStorageAddr) override;

};


#endif //CROSS_PROCESS_HOOK_SOCKETSHELLCODEX86_H
