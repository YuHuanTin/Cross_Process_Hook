#ifndef CROSS_PROCESS_HOOK_SHELLCODEBASE_H
#define CROSS_PROCESS_HOOK_SHELLCODEBASE_H

#include <vector>

class ShellCodeBase {
public:

    virtual std::vector<std::uint8_t> makeShellCode(std::size_t ControlBlockAddr, std::size_t RegisterStorageAddr) = 0;

    virtual ~ShellCodeBase() = default;

protected:
    virtual std::vector<std::uint8_t> &insertRegisterSaveCode(std::vector<std::uint8_t> &CodeData);

    virtual std::vector<std::uint8_t> &insertPushadAndPopad(std::vector<std::uint8_t> &CodeData);

    std::pair<std::size_t, std::size_t> getFuncAddrAndSize(std::size_t FunPtr, const std::uint8_t *EndSign, std::size_t SignSize);

    std::size_t patchAddress(std::uint8_t *DataPtr, std::size_t Begin, std::size_t End, std::size_t Sign, std::size_t PatchValue);
};

#endif //CROSS_PROCESS_HOOK_SHELLCODEBASE_H
