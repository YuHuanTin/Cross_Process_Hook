
#include "Reader_FromFile.h"

bool Reader_FromFile::initSearch(const std::string &DllName) {
    auto processModule = isDllLoaded(DllName);
    if (!processModule)
        return false;

    m_hmodule = LoadLibraryA(processModule->szExePath);
    if (!m_hmodule) return false;
    return true;
}

std::optional<std::size_t> Reader_FromFile::searchRVA(const std::string &FunctionName, bool WithBaseAddress) const {
    auto address = GetProcAddress(m_hmodule, (LPCSTR) FunctionName.c_str());
    if (address != nullptr)
        return (std::size_t) address - (WithBaseAddress ? 0 : (std::size_t) m_hmodule);
    return std::nullopt;
}

std::optional<std::size_t> Reader_FromFile::searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const {
    auto address = GetProcAddress(m_hmodule, (LPCSTR) FunctionOrd);
    if (address != nullptr)
        return (std::size_t) address - (WithBaseAddress ? 0 : (std::size_t) m_hmodule);
    return std::nullopt;
}

Reader_FromFile::~Reader_FromFile() {
    FreeLibrary(m_hmodule);
}