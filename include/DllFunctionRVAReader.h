

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

    DllFunctionRVAReader(DWORD ProcessID, std::string TargetDllName, SearchPoly SearchPoly = SEARCH_IN_FILE);

    DWORD Search(const std::string &FunctionName, bool WithBaseAddress = false);

    DWORD Search(DWORD FunctionOrd, bool WithBaseAddress = false);

    ~DllFunctionRVAReader();

private:
    DWORD       m_processID;
    HANDLE      m_processHandle;
    std::string m_targetDllName;
    SearchPoly  m_searchPoly;

    HMODULE m_hmodule;
    DWORD   m_moduleBaseAddress;

    DWORD                                 m_exportDirectoryOrdBase = 0;
    std::unordered_map<std::string, WORD> m_functionNameToOrd;
    std::unordered_map<WORD, DWORD>       m_functionOrdToRva;
};


#endif //CROSS_PROCESS_HOOK_DLLFUNCTIONRVAREADER_H
