

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
     * SEARCH_IN_MEMORY: ���ڴ��в��Һ��� RVA���п��ܱȽϺ�ʱ��Ϊ��Ҫ����ȫ��������ӳ��
     * SEARCH_IN_FILE: �Ӵ����ϵ� dll �в��Һ��� RVA
     */
    enum SearchPoly {
        SEARCH_IN_MEMORY, SEARCH_IN_FILE
    };

    explicit DllFunctionRVAReader(DWORD ProcessID, SearchPoly SearchPoly = SEARCH_IN_FILE);

    [[nodiscard]] std::optional<MODULEENTRY32> isDllLoaded(const std::string &DllName) const;

    /**
     * ��ʼ����������� false ����� dll δ����
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
