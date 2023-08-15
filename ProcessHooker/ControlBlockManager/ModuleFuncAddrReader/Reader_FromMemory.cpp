

#include "Reader_FromMemory.h"
#include "../../../Utils/Utils.h"
#include "../../../Utils/MyException.h"

struct parseRetType {
    std::unordered_map<std::string, WORD> functionNameToOrd;
    std::unordered_map<WORD, DWORD>       functionOrdToRva;
    DWORD                                 exportDirectoryOrdBase = 0;
};

/**
 * 一定有值，否则异常，不需要判断是否有值
 * 获取 ProcessHandle 进程的以 ModuleBaseAddress 开始的 PE 节地址和大小
 *
 * @param ProcessHandle
 * @param ModuleBaseAddr
 * @return
 */
std::pair<std::size_t, DWORD> getPEAddrWithSize(HANDLE ProcessHandle, std::size_t ModuleBaseAddr) {
    MEMORY_BASIC_INFORMATION basicInformation;

    DWORD retSize = VirtualQueryEx(ProcessHandle, (LPCVOID) ModuleBaseAddr, &basicInformation, sizeof(MEMORY_BASIC_INFORMATION));
    if (retSize == 0) {
        throw MyException("VirtualQueryEx", GetLastError(), __FUNCTION__);
    }
    return {(std::size_t) basicInformation.BaseAddress, basicInformation.RegionSize};
}

/**
 * 一定有值，否则异常，不需要判断是否有值
 * 获取 ProcessHandle 进程的导出表信息（RVA + Size）
 *
 * @param ProcessHandle
 * @param PEAddr
 * @param PESize
 * @return
 */
std::pair<DWORD, DWORD> getExportDirectoryRvaAndSize(HANDLE ProcessHandle, std::size_t PEAddr, DWORD PESize) {
    auto memoryData = Utils::RemoteProcess::readMemory<uint8_t>(ProcessHandle, PEAddr, PESize);

    // 获取 dos 头
    IMAGE_DOS_HEADER dosHeader;
    dosHeader = *(IMAGE_DOS_HEADER *) memoryData.get();
    LONG peOffset = dosHeader.e_lfanew;

    // 获取 pe 头
    IMAGE_NT_HEADERS ntHeaders;
    ntHeaders = *(IMAGE_NT_HEADERS *) (memoryData.get() + peOffset);

    // 获取导出表 rva 和 size
    IMAGE_DATA_DIRECTORY exportDirectory = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    return {exportDirectory.VirtualAddress, exportDirectory.Size};
}

/**
 * 一定有值，否则异常，不需要判断是否有值
 * 解析 ProcessHandle 进程的 PEAddress + ExportDirectoryRVA 处的导出表数据
 *
 * @param ProcessHandle
 * @param PEAddr
 * @param ExportDirectoryRVA
 * @param ExportDirectorySize
 * @return
 */
parseRetType parseExportDirectory(HANDLE ProcessHandle, std::size_t PEAddr, DWORD ExportDirectoryRVA, DWORD ExportDirectorySize) {
    auto exportDirectoryData = Utils::RemoteProcess::readMemory<uint8_t>(ProcessHandle, PEAddr + ExportDirectoryRVA, ExportDirectorySize);

    IMAGE_EXPORT_DIRECTORY exportDirectory;
    exportDirectory = *(IMAGE_EXPORT_DIRECTORY *) exportDirectoryData.get();

    // 将 RVA 地址转换成偏移地址
    DWORD offsetAddressOfFunctions    = exportDirectory.AddressOfFunctions - ExportDirectoryRVA;
    DWORD offsetAddressOfNames        = exportDirectory.AddressOfNames - ExportDirectoryRVA;
    DWORD offsetAddressOfNameOrdinals = exportDirectory.AddressOfNameOrdinals - ExportDirectoryRVA;

    std::unordered_map<std::string, WORD> map_functionNameToOrd(exportDirectory.NumberOfNames);
    std::unordered_map<WORD, DWORD>       map_functionOrdToRVA(exportDirectory.NumberOfFunctions);

    // 建立 函数名 到 序号 的映射
    for (std::size_t i = 0; i < exportDirectory.NumberOfNames; ++i) {
        auto offsetFunctionNames = (DWORD *) (exportDirectoryData.get() + offsetAddressOfNames + i * 4);
        *offsetFunctionNames -= ExportDirectoryRVA;
        std::string functionName = (char *) (exportDirectoryData.get() + *offsetFunctionNames);

        auto offsetFunctionOrds = (WORD *) (exportDirectoryData.get() + offsetAddressOfNameOrdinals + i * 2);
        map_functionNameToOrd[functionName] = *offsetFunctionOrds;
    }
    // 建立 序号 到 函数rva 的映射
    for (std::size_t i = 0; i < exportDirectory.NumberOfFunctions; ++i) {
        auto offsetFunctionOrds     = (WORD *) (exportDirectoryData.get() + offsetAddressOfNameOrdinals + i * 2);
        auto index                  = *offsetFunctionOrds;
        map_functionOrdToRVA[index] = *(DWORD *) (exportDirectoryData.get() + offsetAddressOfFunctions + index * 4);
    }

    return {map_functionNameToOrd, map_functionOrdToRVA, exportDirectory.Base};
}


bool Reader_FromMemory::initSearch(const std::string &DllName) {
    auto processModule = isDllLoaded(DllName);
    if (!processModule)
        return false;
    auto peSegmentAddressWithSize  = getPEAddrWithSize(m_processHandle, (std::size_t) processModule->modBaseAddr);
    auto exportDirectoryRvaAndSize = getExportDirectoryRvaAndSize(m_processHandle, peSegmentAddressWithSize.first, peSegmentAddressWithSize.second);
    auto parseResult               = parseExportDirectory(m_processHandle, peSegmentAddressWithSize.first, exportDirectoryRvaAndSize.first,
                                                          exportDirectoryRvaAndSize.second);

    m_moduleBaseAddr         = processModule->modBaseAddr;
    m_functionNameToOrd      = parseResult.functionNameToOrd;
    m_functionOrdToRva       = parseResult.functionOrdToRva;
    m_exportDirectoryOrdBase = parseResult.exportDirectoryOrdBase;

    return true;
}

std::optional<std::size_t> Reader_FromMemory::searchRVA(const std::string &FunctionName, bool WithBaseAddress) const {
    auto it = m_functionNameToOrd.find(FunctionName);
    if (it != m_functionNameToOrd.end())
        return m_functionOrdToRva.at(it->second) + (std::size_t) (WithBaseAddress ? m_moduleBaseAddr : 0);
    return std::nullopt;
}

std::optional<std::size_t> Reader_FromMemory::searchRVA(DWORD FunctionOrd, bool WithBaseAddress) const {
    auto it = m_functionOrdToRva.find(FunctionOrd - m_exportDirectoryOrdBase);
    if (it != m_functionOrdToRva.end())
        return it->second + (std::size_t) (WithBaseAddress ? m_moduleBaseAddr : 0);
    return std::nullopt;
}
