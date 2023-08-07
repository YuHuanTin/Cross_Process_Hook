

#ifndef CROSS_PROCESS_HOOK_SOCKETCONTROLBLOCK_H
#define CROSS_PROCESS_HOOK_SOCKETCONTROLBLOCK_H

#include "ControlBlockBase.h"

class SocketControlBlock : public ControlBlockBase {
public:
    using ControlBlockBase::ControlBlockBase;

    void fillControlBlockArgs() override;
};


#endif //CROSS_PROCESS_HOOK_SOCKETCONTROLBLOCK_H
