

#ifndef CROSS_PROCESS_HOOK_SHELLCODE_SOCKET_UDP_X86_H
#define CROSS_PROCESS_HOOK_SHELLCODE_SOCKET_UDP_X86_H

#include "ShellCodeBase.h"

class ShellCode_Socket_UDP_X86 : public ShellCodeBase {
public:
    std::vector<std::uint8_t> makeShellCode(std::size_t ControlBlockAddr, std::size_t RegisterStorageAddr) override;

};


#endif //CROSS_PROCESS_HOOK_SHELLCODE_SOCKET_UDP_X86_H
