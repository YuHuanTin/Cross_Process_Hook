

#ifndef CROSS_PROCESS_HOOK_RVAREADERBASE_H
#define CROSS_PROCESS_HOOK_RVAREADERBASE_H


#include <string>
#include <optional>
#include <Windows.h>
#include <TlHelp32.h>

class RVAReaderBase {
public:
    explicit RVAReaderBase(DWORD ProcessID);

    /**
     * ���ָ�� dll �ڽ������Ƿ����
     *
     * @param DllName
     * @return
     */
    [[nodiscard]] std::optional<MODULEENTRY32> isDllLoaded(const std::string &DllName) const;

    /**
     * ��ʼ����������� false ����� dll δ����
     *
     * @return
     */
    virtual bool initSearch(const std::string &DllName) = 0;

    /**
     * ���պ��������� RVA
     *
     * @param FunctionName
     * @param WithBaseAddress
     * @return
     */
    [[nodiscard]] virtual std::optional<DWORD> searchRVA(const std::string &FunctionName, bool WithBaseAddress) const = 0;

    /**
     * ������Ų��� RVA
     *
     * @param FunctionOrd
     * @param WithBaseAddress
     * @return
     */
    [[nodiscard]] virtual std::optional<DWORD> searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const = 0;

    virtual ~RVAReaderBase();

protected:
    DWORD  m_processID     = 0;
    HANDLE m_processHandle = INVALID_HANDLE_VALUE;
};


#endif //CROSS_PROCESS_HOOK_RVAREADERBASE_H
