

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCK_SOCKET_UDP_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCK_SOCKET_UDP_H

#include "ControlBlockBase.h"

class ControlBlock_Socket_UDP : public ControlBlockBase {
public:
    using ControlBlockBase::ControlBlockBase;
    
    void fillControlBlockArgs() override;
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCK_SOCKET_UDP_H
