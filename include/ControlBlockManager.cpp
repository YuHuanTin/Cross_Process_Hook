
#include "ControlBlockManager.h"
#include "DllFunctionRVAReader.h"
#include "../Utils/Utils.h"

ControlBlockManager::ControlBlockManager(DWORD ProcessID)
        : m_controlBlockPtr(std::make_unique<ControlBlock>()),
          m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)) {}

ControlBlockManager::~ControlBlockManager() {
    // free remote process loaded dll because hook from m_hookLoadDlls!
    DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

    for (const auto &one: m_hookLoadDlls) {
        if (rvaReader.isDllLoaded(one)){
            rvaReader.freeDll(one, m_freeLibraryAddr);
        }
    }

    // free remote process memory controlblock!
    if (m_controlBlockRemoteAddr) {
        Utils::RemoteProcess::freeMemory(m_processHandle, m_controlBlockRemoteAddr.value());
    }
    CloseHandle(m_processHandle);
}

void ControlBlockManager::fillSocketArgs() {
    DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

    const char *dlls[] = {"kernelbase.dll", "ws2_32.dll", "ucrtbase.dll"};

    /// kernelbase.dll
    if (!rvaReader.isDllLoaded(dlls[0]) || !rvaReader.initSearch(dlls[0])) {
        // 不能自动加载所缺 dll 因为 LoadLibraryA 无法获取到
        throw std::runtime_error("unloadDll kernelbase");
    }
    DWORD loadLibraryAddr = rvaReader.searchRVA("LoadLibraryA", true);
    m_freeLibraryAddr = rvaReader.searchRVA("FreeLibrary", true);

    /// ws2_32.dll
    if (!rvaReader.isDllLoaded(dlls[1])) {
        if (!rvaReader.loadDll(dlls[1], loadLibraryAddr)) {
            throw std::runtime_error("unloadDll ws2_32");
        }
        m_hookLoadDlls.emplace_back(dlls[1]);
    }
    if (!rvaReader.initSearch(dlls[1])) {
        throw std::runtime_error("initSearch ws2_32");
    }

    m_controlBlockPtr->HookMethod                         = HookMethod::Socket;
    m_controlBlockPtr->PSocketFunctionAddress.WSAStartup  = rvaReader.searchRVA("WSAStartup", true);
    m_controlBlockPtr->PSocketFunctionAddress.WSACleanup  = rvaReader.searchRVA("WSACleanup", true);
    m_controlBlockPtr->PSocketFunctionAddress.socket      = rvaReader.searchRVA("socket", true);
    m_controlBlockPtr->PSocketFunctionAddress.connect     = rvaReader.searchRVA("connect", true);
    m_controlBlockPtr->PSocketFunctionAddress.send        = rvaReader.searchRVA("send", true);
    m_controlBlockPtr->PSocketFunctionAddress.recv        = rvaReader.searchRVA("recv", true);
    m_controlBlockPtr->PSocketFunctionAddress.closesocket = rvaReader.searchRVA("closesocket", true);
    m_controlBlockPtr->PSocketFunctionAddress.htons       = rvaReader.searchRVA("htons", true);
    m_controlBlockPtr->PSocketFunctionAddress.inet_addr   = rvaReader.searchRVA("inet_addr", true);

    /// ucrtbase.dll
    if (!rvaReader.isDllLoaded(dlls[2])) {
        if (!rvaReader.loadDll(dlls[2], loadLibraryAddr)) {
            throw std::runtime_error("unloadDll ucrtbase");
        }
        m_hookLoadDlls.emplace_back(dlls[2]);
    }
    if (!rvaReader.initSearch(dlls[2])) {
        throw std::runtime_error("initSearch ucrtbase");
    }

    m_controlBlockPtr->PSocketFunctionAddress.malloc = rvaReader.searchRVA("malloc", true);
    m_controlBlockPtr->PSocketFunctionAddress.free   = rvaReader.searchRVA("free", true);
}

void ControlBlockManager::injectControlBlock(HANDLE ProcessHandle) {
    m_controlBlockRemoteAddr = Utils::RemoteProcess::allocMemory(ProcessHandle);
    if (!m_controlBlockRemoteAddr)
        throw std::runtime_error("allocMemory");
    if (!Utils::RemoteProcess::writeMemory(ProcessHandle, m_controlBlockRemoteAddr.value(), m_controlBlockPtr.get(), sizeof(ControlBlock)))
        throw std::runtime_error("writeMemory");
}

std::optional<LPVOID> ControlBlockManager::getRemoteControlBlockAddr() {
    return m_controlBlockRemoteAddr;
}

