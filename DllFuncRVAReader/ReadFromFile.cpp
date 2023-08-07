
#include "ReadFromFile.h"

bool ReadFromFile::initSearch(const std::string &DllName) {
    auto processModule = isDllLoaded(DllName);
    if (!processModule)
        return false;

    m_hmodule = LoadLibraryA(processModule->szExePath);
    if (!m_hmodule) return false;
    return true;
}

std::optional<DWORD> ReadFromFile::searchRVA(const std::string &FunctionName, bool WithBaseAddress) const {
    auto address = GetProcAddress(m_hmodule, (LPCSTR) FunctionName.c_str());
    if (address != nullptr)
        return (DWORD) address - (WithBaseAddress ? 0 : (DWORD) m_hmodule);
    return std::nullopt;
}

std::optional<DWORD> ReadFromFile::searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const {
    auto address = GetProcAddress(m_hmodule, (LPCSTR) FunctionOrd);
    if (address != nullptr)
        return (DWORD) address - (WithBaseAddress ? 0 : (DWORD) m_hmodule);
    return std::nullopt;
}

ReadFromFile::~ReadFromFile() {
    FreeLibrary(m_hmodule);
}