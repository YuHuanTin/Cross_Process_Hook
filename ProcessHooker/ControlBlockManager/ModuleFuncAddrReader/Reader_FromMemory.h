

#ifndef CROSS_PROCESS_HOOK_READER_FROMMEMORY_H
#define CROSS_PROCESS_HOOK_READER_FROMMEMORY_H

#include <unordered_map>
#include "ReaderBase.h"

class Reader_FromMemory : public ReaderBase {
public:
    using ReaderBase::ReaderBase;

    bool initSearch(const std::string &DllName) override;

    [[nodiscard]] std::optional<std::size_t> searchRVA(const std::string &FunctionName, bool WithBaseAddress) const override;

    [[nodiscard]] std::optional<std::size_t> searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const override;

    ~Reader_FromMemory() override = default;

private:
    LPVOID                                m_moduleBaseAddr         = nullptr;
    DWORD                                 m_exportDirectoryOrdBase = 0;
    std::unordered_map<std::string, WORD> m_functionNameToOrd;
    std::unordered_map<WORD, DWORD>       m_functionOrdToRva;
};


#endif //CROSS_PROCESS_HOOK_READER_FROMMEMORY_H
