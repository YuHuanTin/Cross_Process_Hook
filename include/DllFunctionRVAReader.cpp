

#include "DllFunctionRVAReader.h"
#include "../Utils/AutoMemoryManager.h"

struct parseRetType {
    std::unordered_map<std::string, WORD> functionNameToOrd;
    std::unordered_map<WORD, DWORD>       functionOrdToRva;
    DWORD                                 exportDirectoryOrdBase = 0;
};

/**
 * ��ȡ PID ���̵� DllName ��ģ���ַ��ģ���ܴ�С
 *
 * @param ProcessID
 * @param DllName
 * @return
 */
std::optional<std::pair<DWORD, DWORD>> getProcessModuleBaseAddressWithSize(DWORD ProcessID, const std::string &DllName) {
    DWORD  moduleBaseAddress = 0;
    DWORD  moduleSize        = 0;
    HANDLE hSnapshot         = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return std::nullopt;

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            std::string module = moduleEntry.szModule;
            if (module == DllName) {
                moduleBaseAddress = (DWORD) moduleEntry.modBaseAddr;
                moduleSize        = (DWORD) moduleEntry.modBaseSize;
                break;
            }
        } while (Module32Next(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    return {{moduleBaseAddress, moduleSize}};
}

/**
 * ��ȡ ProcessHandle ���̵��� ModuleBaseAddress ��ʼ�� PE �ڵ�ַ�ʹ�С
 *
 * @param ProcessHandle
 * @param ModuleBaseAddress
 * @return
 */
std::optional<std::pair<DWORD, DWORD>> getPEAddressWithSize(HANDLE ProcessHandle, DWORD ModuleBaseAddress) {
    MEMORY_BASIC_INFORMATION basicInformation;
    DWORD                    retSize = VirtualQueryEx(ProcessHandle, (LPCVOID) ModuleBaseAddress, &basicInformation, sizeof(MEMORY_BASIC_INFORMATION));
    if (retSize == 0) {
        return std::nullopt;
    }
    return {{(DWORD) basicInformation.BaseAddress, basicInformation.RegionSize}};
}

/**
 * ��ȡ ProcessHandle ���̵ĵ�������Ϣ��RVA + Size��
 *
 * @param ProcessHandle
 * @param PEAddress
 * @param PESize
 * @return
 */
std::optional<std::pair<DWORD, DWORD>> getExportDirectoryRvaAndSize(HANDLE ProcessHandle, DWORD PEAddress, DWORD PESize) {
    auto data = Utils::getProcessMemoryDataByReadProcess<uint8_t>(ProcessHandle, PEAddress, PESize);
    if (!data) return {};

    // ��ȡ dos ͷ
    IMAGE_DOS_HEADER dosHeader;
    dosHeader = *(IMAGE_DOS_HEADER *) data.get();
    LONG peOffset = dosHeader.e_lfanew;

    // ��ȡ pe ͷ
    IMAGE_NT_HEADERS ntHeaders;
    ntHeaders = *(IMAGE_NT_HEADERS *) (data.get() + peOffset);

    // ��ȡ������ rva �� size
    IMAGE_DATA_DIRECTORY exportDirectory = ntHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    return {{exportDirectory.VirtualAddress, exportDirectory.Size}};
}

/**
 * ��ȡ PIP ���� DllName ģ��Ĵ���·����ģ���ַ
 *
 * @param ProcessID
 * @param DllName
 * @return
 */
std::optional<std::pair<std::string, DWORD>> getProcessModulePathWithBaseAddress(DWORD ProcessID, const std::string &DllName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return std::nullopt;

    std::string modulePath;
    DWORD       baseAddress;

    MODULEENTRY32 moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &moduleEntry)) {
        do {
            std::string module = moduleEntry.szModule;
            if (module == DllName) {
                modulePath  = moduleEntry.szExePath;
                baseAddress = (DWORD) moduleEntry.modBaseAddr;
                break;
            }
        } while (Module32Next(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    return {{modulePath, baseAddress}};
}

/**
 * ���� ProcessHandle ���̵� PEAddress + ExportDirectoryRVA ���ĵ���������
 *
 * @param ProcessHandle
 * @param PEAddress
 * @param ExportDirectoryRVA
 * @param ExportDirectorySize
 * @return
 */
std::optional<parseRetType> parseExportDirectory(HANDLE ProcessHandle, DWORD PEAddress, DWORD ExportDirectoryRVA, DWORD ExportDirectorySize) {
    auto exportDirectoryData = Utils::getProcessMemoryDataByReadProcess<uint8_t>(ProcessHandle, PEAddress + ExportDirectoryRVA, ExportDirectorySize);
    if (!exportDirectoryData) return std::nullopt;

    IMAGE_EXPORT_DIRECTORY exportDirectory;
    exportDirectory = *(IMAGE_EXPORT_DIRECTORY *) exportDirectoryData.get();

    // �� RVA ��ַת����ƫ�Ƶ�ַ
    DWORD offsetAddressOfFunctions    = exportDirectory.AddressOfFunctions - ExportDirectoryRVA;
    DWORD offsetAddressOfNames        = exportDirectory.AddressOfNames - ExportDirectoryRVA;
    DWORD offsetAddressOfNameOrdinals = exportDirectory.AddressOfNameOrdinals - ExportDirectoryRVA;

    std::unordered_map<std::string, WORD> map_functionNameToOrd(exportDirectory.NumberOfNames);
    std::unordered_map<WORD, DWORD>       map_functionOrdToRVA(exportDirectory.NumberOfFunctions);

    // ���� ������ �� ��� ��ӳ��
    for (std::size_t i = 0; i < exportDirectory.NumberOfNames; ++i) {
        auto offsetFunctionNames = (DWORD *) (exportDirectoryData.get() + offsetAddressOfNames + i * 4);
        *offsetFunctionNames -= ExportDirectoryRVA;
        std::string functionName = (char *) (exportDirectoryData.get() + *offsetFunctionNames);

        auto offsetFunctionOrds = (WORD *) (exportDirectoryData.get() + offsetAddressOfNameOrdinals + i * 2);
        map_functionNameToOrd[functionName] = *offsetFunctionOrds;
    }
    // ���� ��� �� ����rva ��ӳ��
    for (std::size_t i = 0; i < exportDirectory.NumberOfFunctions; ++i) {
        auto offsetFunctionOrds     = (WORD *) (exportDirectoryData.get() + offsetAddressOfNameOrdinals + i * 2);
        auto index                  = *offsetFunctionOrds;
        map_functionOrdToRVA[index] = *(DWORD *) (exportDirectoryData.get() + offsetAddressOfFunctions + index * 4);
    }

    return {{map_functionNameToOrd, map_functionOrdToRVA, exportDirectory.Base}};
}


DllFunctionRVAReader::DllFunctionRVAReader(DWORD ProcessID, std::string TargetDllName, SearchPoly SearchPoly)
        : m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_targetDllName(std::move(TargetDllName)),
          m_searchPoly(SearchPoly) {
    if (m_searchPoly == SEARCH_IN_MEMORY) {
        auto processModuleBaseAddressWithSize = getProcessModuleBaseAddressWithSize(ProcessID, m_targetDllName);
        if (!processModuleBaseAddressWithSize) throw std::runtime_error("getProcessModuleBaseAddressWithSize");

        auto peSegmentAddressWithSize = getPEAddressWithSize(m_processHandle, processModuleBaseAddressWithSize->first);
        if (!peSegmentAddressWithSize) throw std::runtime_error("getPEAddressWithSize");

        auto exportDirectoryRvaAndSize = getExportDirectoryRvaAndSize(m_processHandle, peSegmentAddressWithSize->first, peSegmentAddressWithSize->second);
        if (!exportDirectoryRvaAndSize) throw std::runtime_error("getExportDirectoryRvaAndSize");

        auto parseResult = parseExportDirectory(m_processHandle, peSegmentAddressWithSize->first, exportDirectoryRvaAndSize->first,
                                                exportDirectoryRvaAndSize->second);
        if (!parseResult) throw std::runtime_error("parseExportDirectory");

        m_moduleBaseAddress      = processModuleBaseAddressWithSize->first;
        m_functionNameToOrd      = parseResult->functionNameToOrd;
        m_functionOrdToRva       = parseResult->functionOrdToRva;
        m_exportDirectoryOrdBase = parseResult->exportDirectoryOrdBase;

    } else if (m_searchPoly == SEARCH_IN_FILE) {
        auto modulePathWithBaseAddress = getProcessModulePathWithBaseAddress(m_processID, m_targetDllName);
        if (!modulePathWithBaseAddress) throw std::runtime_error("getProcessModulePathWithBaseAddress");

        m_hmodule = LoadLibraryA(modulePathWithBaseAddress->first.c_str());
        if (!m_hmodule) throw std::runtime_error("LoadLibraryA");

        m_moduleBaseAddress = (DWORD)m_hmodule;
    }
}

DWORD DllFunctionRVAReader::Search(const std::string &FunctionName, bool WithBaseAddress) {
    if (m_searchPoly == SEARCH_IN_MEMORY) {
        auto it = m_functionNameToOrd.find(FunctionName);
        if (it == m_functionNameToOrd.end()) {
            return 0;
        }
        return m_functionOrdToRva[it->second] + (WithBaseAddress ? m_moduleBaseAddress : 0);
    } else if (m_searchPoly == SEARCH_IN_FILE) {
        auto address = (DWORD) GetProcAddress(m_hmodule, (LPCSTR) FunctionName.c_str());
        if (address != 0) {
            return address - (WithBaseAddress ? 0 : m_moduleBaseAddress);
        }
    }
    return 0;
}

DWORD DllFunctionRVAReader::Search(DWORD FunctionOrd, bool WithBaseAddress) {
    if (m_searchPoly == SEARCH_IN_MEMORY) {
        return m_functionOrdToRva[FunctionOrd - m_exportDirectoryOrdBase] + (WithBaseAddress ? m_moduleBaseAddress : 0);
    } else if (m_searchPoly == SEARCH_IN_FILE) {
        auto address = (DWORD) GetProcAddress(m_hmodule, (LPCSTR) FunctionOrd);
        if (address!= 0) {
            return address - (WithBaseAddress ? 0 : m_moduleBaseAddress);
        }
    }
    return 0;
}

DllFunctionRVAReader::~DllFunctionRVAReader() {
    CloseHandle(m_processHandle);
}

