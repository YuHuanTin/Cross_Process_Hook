
#include "ControlBlockManager.h"
#include "DllFunctionRVAReader.h"
#include "../Utils/Utils.h"

ControlBlockManager::ControlBlockManager(DWORD ProcessID)
        : m_controlBlockPtr(std::make_unique<ControlBlock>()),
          m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_loadLibraryAddr(0),
          m_freeLibraryAddr(0),
          m_controlBlockRemoteAddr(nullptr) {}

ControlBlockManager::~ControlBlockManager() {
    /// 释放目标进程 hook 所需而加载的 dll
    DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

    for (const auto &one: m_hookLoadDlls) {
        if (rvaReader.isDllLoaded(one)) {
            Utils::RemoteProcess::freeDll(m_processHandle, m_freeLibraryAddr, Utils::RemoteProcess::getProcessModule(m_processID, one).value().hModule);
        }
    }

    /// 释放目标进程的控制块内存
    if (m_controlBlockRemoteAddr) {
        Utils::RemoteProcess::freeMemory(m_processHandle, m_controlBlockRemoteAddr);
    }
    CloseHandle(m_processHandle);
}

void ControlBlockManager::fillSocketArgs() {
    // 如果找不到指定函数的地址则抛出异常
    try {
        DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

        const char *dlls[] = {"kernelbase.dll", "ws2_32.dll", "ucrtbase.dll"};

        /// kernelbase.dll
        if (!rvaReader.isDllLoaded(dlls[0]) || !rvaReader.initSearch(dlls[0])) {
            // 不能自动加载所缺 dll 因为 LoadLibraryA 无法获取到
            throw MyException("unloadDll kernelbase", __FUNCTION__);
        }

        m_loadLibraryAddr = rvaReader.searchRVA("LoadLibraryA", true).value();
        m_freeLibraryAddr = rvaReader.searchRVA("FreeLibrary", true).value();

        /// ws2_32.dll
        if (!rvaReader.isDllLoaded(dlls[1])) {
            Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[1]);
            m_hookLoadDlls.emplace_back(dlls[1]);
        }
        if (!rvaReader.initSearch(dlls[1])) {
            throw MyException("initSearch ws2_32", __FUNCTION__);
        }

        m_controlBlockPtr->HookMethod                         = HookMethod::Socket;
        m_controlBlockPtr->PSocketFunctionAddress.WSAStartup  = rvaReader.searchRVA("WSAStartup", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.WSACleanup  = rvaReader.searchRVA("WSACleanup", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.socket      = rvaReader.searchRVA("socket", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.connect     = rvaReader.searchRVA("connect", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.send        = rvaReader.searchRVA("send", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.recv        = rvaReader.searchRVA("recv", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.closesocket = rvaReader.searchRVA("closesocket", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.htons       = rvaReader.searchRVA("htons", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.inet_addr   = rvaReader.searchRVA("inet_addr", true).value();

        /// ucrtbase.dll
        if (!rvaReader.isDllLoaded(dlls[2])) {
            Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[2]);
            m_hookLoadDlls.emplace_back(dlls[2]);
        }
        if (!rvaReader.initSearch(dlls[2])) {
            throw MyException("initSearch ucrtbase", __FUNCTION__);
        }

        m_controlBlockPtr->PSocketFunctionAddress.malloc = rvaReader.searchRVA("malloc", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.free   = rvaReader.searchRVA("free", true).value();
    } catch (std::bad_optional_access &bad_optional_access) {
        throw MyException("can not find function RVA", __FUNCTION__);
    }
}

void ControlBlockManager::injectControlBlock(HANDLE ProcessHandle)  {
    m_controlBlockRemoteAddr = Utils::RemoteProcess::allocMemory(ProcessHandle);
    Utils::RemoteProcess::writeMemory(m_processHandle, m_controlBlockRemoteAddr, m_controlBlockPtr.get(), sizeof(ControlBlock));
}

LPVOID ControlBlockManager::getRemoteControlBlockAddr() const noexcept {
    return m_controlBlockRemoteAddr;
}

