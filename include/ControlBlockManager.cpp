
#include "ControlBlockManager.h"
#include "../Utils/Utils.h"

ControlBlockManager::ControlBlockManager(DWORD ProcessID)
        : m_controlBlockPtr(std::make_unique<ControlBlock>()),
          m_rvaReader(std::make_unique<ReadFromFile>(ProcessID)),
          m_processID(ProcessID),
          m_processHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_processID)),
          m_loadLibraryAddr(0),
          m_freeLibraryAddr(0),
          m_controlBlockRemoteAddr(nullptr) {}

ControlBlockManager::~ControlBlockManager() {
    /// 释放目标进程 hook 所需而加载的 dll
    for (const auto &one: m_hookLoadDlls) {
        if (m_rvaReader->isDllLoaded(one)) {
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
        const char *dlls[] = {"kernelbase.dll", "ws2_32.dll", "ucrtbase.dll"};

        /// kernelbase.dll
        if (!m_rvaReader->isDllLoaded(dlls[0]) || !m_rvaReader->initSearch(dlls[0])) {
            // 不能自动加载所缺 dll 因为 LoadLibraryA 无法获取到
            throw MyException("unloadDll kernelbase", __FUNCTION__);
        }

        m_loadLibraryAddr = m_rvaReader->searchRVA("LoadLibraryA", true).value();
        m_freeLibraryAddr = m_rvaReader->searchRVA("FreeLibrary", true).value();

        /// ws2_32.dll
        if (!m_rvaReader->isDllLoaded(dlls[1])) {
            Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[1]);
            m_hookLoadDlls.emplace_back(dlls[1]);
        }
        if (!m_rvaReader->initSearch(dlls[1])) {
            throw MyException("initSearch ws2_32", __FUNCTION__);
        }

        m_controlBlockPtr->HookMethod                         = HookMethod::Socket;
        m_controlBlockPtr->PSocketFunctionAddress.WSAStartup  = m_rvaReader->searchRVA("WSAStartup", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.WSACleanup  = m_rvaReader->searchRVA("WSACleanup", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.socket      = m_rvaReader->searchRVA("socket", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.connect     = m_rvaReader->searchRVA("connect", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.send        = m_rvaReader->searchRVA("send", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.recv        = m_rvaReader->searchRVA("recv", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.closesocket = m_rvaReader->searchRVA("closesocket", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.htons       = m_rvaReader->searchRVA("htons", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.inet_addr   = m_rvaReader->searchRVA("inet_addr", true).value();

        /// ucrtbase.dll
        if (!m_rvaReader->isDllLoaded(dlls[2])) {
            Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[2]);
            m_hookLoadDlls.emplace_back(dlls[2]);
        }
        if (!m_rvaReader->initSearch(dlls[2])) {
            throw MyException("initSearch ucrtbase", __FUNCTION__);
        }

        m_controlBlockPtr->PSocketFunctionAddress.malloc = m_rvaReader->searchRVA("malloc", true).value();
        m_controlBlockPtr->PSocketFunctionAddress.free   = m_rvaReader->searchRVA("free", true).value();
    } catch (std::bad_optional_access &bad_optional_access) {
        throw MyException("can not find function RVA", __FUNCTION__);
    }
}

void ControlBlockManager::injectControlBlock(HANDLE ProcessHandle) {
    m_controlBlockRemoteAddr = Utils::RemoteProcess::allocMemory(ProcessHandle);
    Utils::RemoteProcess::writeMemory(m_processHandle, m_controlBlockRemoteAddr, m_controlBlockPtr.get(), sizeof(ControlBlock));
}

LPVOID ControlBlockManager::getRemoteControlBlockAddr() const noexcept {
    return m_controlBlockRemoteAddr;
}

