

#ifndef CROSS_PROCESS_HOOK_DLLFUNCTIONRVAREADER_H
#define CROSS_PROCESS_HOOK_DLLFUNCTIONRVAREADER_H


#include <string>
#include <utility>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <Windows.h>
#include <TlHelp32.h>

class DllFunctionRVAReader {
public:
    /**
     * SEARCH_IN_MEMORY: 从内存中查找函数 RVA，有可能比较耗时因为需要建立全部函数的映射
     * SEARCH_IN_FILE: 从磁盘上的 dll 中查找函数 RVA
     */
    enum SearchPoly {
        SEARCH_IN_MEMORY, SEARCH_IN_FILE
    };

    explicit DllFunctionRVAReader(DWORD ProcessID, SearchPoly SearchPoly = SEARCH_IN_FILE);

    [[nodiscard]] std::optional<MODULEENTRY32> isDllLoaded(const std::string &DllName) const;

    /**
     * 初始化搜索，如果 false 则可能 dll 未加载
     *
     * @return
     */
    bool initSearch(const std::string &DllName);

    std::optional<DWORD> searchRVA(const std::string &FunctionName, bool WithBaseAddress = false) const;

    std::optional<DWORD> searchRVA(DWORD FunctionOrd, bool WithBaseAddress = false) const;

    ~DllFunctionRVAReader();

private:
    DWORD       m_processID     = 0;
    HANDLE      m_processHandle = INVALID_HANDLE_VALUE;
    SearchPoly  m_searchPoly;

    HMODULE m_hmodule = nullptr;
    DWORD   m_moduleBaseAddress = 0;

    DWORD                                 m_exportDirectoryOrdBase = 0;
    std::unordered_map<std::string, WORD> m_functionNameToOrd;
    std::unordered_map<WORD, DWORD>       m_functionOrdToRva;
};


#endif //CROSS_PROCESS_HOOK_DLLFUNCTIONRVAREADER_H
