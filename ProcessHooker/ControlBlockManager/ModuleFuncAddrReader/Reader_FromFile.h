

#ifndef CROSS_PROCESS_HOOK_READER_FROMFILE_H
#define CROSS_PROCESS_HOOK_READER_FROMFILE_H

#include "ReaderBase.h"

class Reader_FromFile : public ReaderBase {
public:
    using ReaderBase::ReaderBase;

    bool initSearch(const std::string &DllName) override;

    [[nodiscard]] std::optional<std::size_t> searchRVA(const std::string &FunctionName, bool WithBaseAddress) const override;

    [[nodiscard]] std::optional<std::size_t> searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const override;

    ~Reader_FromFile() override;

private:
    HMODULE m_hmodule = nullptr;
};


#endif //CROSS_PROCESS_HOOK_READER_FROMFILE_H
