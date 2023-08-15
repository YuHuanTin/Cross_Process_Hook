

#ifndef CROSS_PROCESS_HOOK_CONTROLBLOCK_SOCKET_TCP_H
#define CROSS_PROCESS_HOOK_CONTROLBLOCK_SOCKET_TCP_H

#include "ControlBlockBase.h"

class ControlBlock_Socket_TCP : public ControlBlockBase {
public:
    using ControlBlockBase::ControlBlockBase;

    void fillControlBlockArgs() override;
};


#endif //CROSS_PROCESS_HOOK_CONTROLBLOCK_SOCKET_TCP_H
