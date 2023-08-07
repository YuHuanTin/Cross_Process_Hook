

#ifndef CROSS_PROCESS_HOOK_READFROMFILE_H
#define CROSS_PROCESS_HOOK_READFROMFILE_H

#include "RVAReaderBase.h"

class ReadFromFile : public RVAReaderBase {
public:
    using RVAReaderBase::RVAReaderBase;

    bool initSearch(const std::string &DllName) override;

    [[nodiscard]] std::optional<DWORD> searchRVA(const std::string &FunctionName, bool WithBaseAddress) const override;

    [[nodiscard]] std::optional<DWORD> searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const override;

    ~ReadFromFile() override;

private:
    HMODULE m_hmodule           = nullptr;
};


#endif //CROSS_PROCESS_HOOK_READFROMFILE_H
