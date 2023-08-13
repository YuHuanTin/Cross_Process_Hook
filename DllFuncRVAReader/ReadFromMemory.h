

#ifndef CROSS_PROCESS_HOOK_READFROMMEMORY_H
#define CROSS_PROCESS_HOOK_READFROMMEMORY_H

#include <unordered_map>
#include "RVAReaderBase.h"

class ReadFromMemory : public RVAReaderBase {
public:
    using RVAReaderBase::RVAReaderBase;

    bool initSearch(const std::string &DllName) override;

    [[nodiscard]] std::optional<LPVOID> searchRVA(const std::string &FunctionName, bool WithBaseAddress) const override;

    [[nodiscard]] std::optional<LPVOID> searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const override;

    ~ReadFromMemory() override = default;

private:
    LPVOID                                m_moduleBaseAddr         = nullptr;
    DWORD                                 m_exportDirectoryOrdBase = 0;
    std::unordered_map<std::string, WORD> m_functionNameToOrd;
    std::unordered_map<WORD, DWORD>       m_functionOrdToRva;
};


#endif //CROSS_PROCESS_HOOK_READFROMMEMORY_H
