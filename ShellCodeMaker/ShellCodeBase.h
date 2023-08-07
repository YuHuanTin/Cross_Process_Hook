#ifndef CROSS_PROCESS_HOOK_SHELLCODEBASE_H
#define CROSS_PROCESS_HOOK_SHELLCODEBASE_H

#include <vector>
#include <Windows.h>

class ShellCodeBase {
public:

    virtual std::vector<std::uint8_t> makeShellCode(DWORD ControlBlockAddr, DWORD RegisterStorageAddr) = 0;

protected:
    virtual std::vector<std::uint8_t> &insertRegisterSaveCode(std::vector<std::uint8_t> &CodeData);

    virtual std::vector<std::uint8_t> &insertPushadAndPopad(std::vector<std::uint8_t> &CodeData);

    std::pair<DWORD, DWORD> getFuncAddressAndSize(DWORD FunPtr, const std::uint8_t *EndSign, std::size_t SignSize);

    DWORD patchAddress(std::uint8_t *DataPtr, std::size_t Begin, std::size_t End, DWORD Sign, DWORD PatchValue);
};

#endif //CROSS_PROCESS_HOOK_SHELLCODEBASE_H
