#ifndef CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
#define CROSS_PROCESS_HOOK_SHELLCODEMAKER_H

#include <vector>
#include <optional>
#include <stdexcept>
#include <WinSock2.h>


class ShellCodeMaker {
private:

    DWORD m_processID;

public:
    ShellCodeMaker();
    std::vector<std::uint8_t> makeSocketShellCode(DWORD DataBufferBlockAddr, DWORD RegisterStorageAddr);


};

#endif //CROSS_PROCESS_HOOK_SHELLCODEMAKER_H
