
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
    /// �ͷ�Ŀ����� hook ��������ص� dll
    DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

    for (const auto &one: m_hookLoadDlls) {
        if (rvaReader.isDllLoaded(one)) {
            Utils::RemoteProcess::freeDll(m_processHandle, m_freeLibraryAddr, Utils::RemoteProcess::getProcessModule(m_processID, one).value().hModule);
        }
    }

    /// �ͷ�Ŀ����̵Ŀ��ƿ��ڴ�
    if (m_controlBlockRemoteAddr) {
        Utils::RemoteProcess::freeMemory(m_processHandle, m_controlBlockRemoteAddr);
    }
    CloseHandle(m_processHandle);
}

void ControlBlockManager::fillSocketArgs() {
    DllFunctionRVAReader rvaReader(m_processID, DllFunctionRVAReader::SEARCH_IN_FILE);

    const char *dlls[] = {"kernelbase.dll", "ws2_32.dll", "ucrtbase.dll"};

    /// kernelbase.dll
    if (!rvaReader.isDllLoaded(dlls[0]) || !rvaReader.initSearch(dlls[0])) {
        // �����Զ�������ȱ dll ��Ϊ LoadLibraryA �޷���ȡ��
        throw MyException("unloadDll kernelbase", __FUNCTION__);
    }

    m_loadLibraryAddr = rvaReader.searchRVA("LoadLibraryA", true);
    m_freeLibraryAddr = rvaReader.searchRVA("FreeLibrary", true);


    /// ws2_32.dll
    if (!rvaReader.isDllLoaded(dlls[1])) {
        Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[1]);
        m_hookLoadDlls.emplace_back(dlls[1]);
    }
    if (!rvaReader.initSearch(dlls[1])) {
        throw MyException("initSearch ws2_32", __FUNCTION__);
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
        Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[2]);
        m_hookLoadDlls.emplace_back(dlls[2]);
    }
    if (!rvaReader.initSearch(dlls[2])) {
        throw MyException("initSearch ucrtbase", __FUNCTION__);
    }

    m_controlBlockPtr->PSocketFunctionAddress.malloc = rvaReader.searchRVA("malloc", true);
    m_controlBlockPtr->PSocketFunctionAddress.free   = rvaReader.searchRVA("free", true);
}

void ControlBlockManager::injectControlBlock(HANDLE ProcessHandle)  {
    m_controlBlockRemoteAddr = Utils::RemoteProcess::allocMemory(ProcessHandle);
    Utils::RemoteProcess::writeMemory(m_processHandle, m_controlBlockRemoteAddr, m_controlBlockPtr.get(), sizeof(ControlBlock));
}

LPVOID ControlBlockManager::getRemoteControlBlockAddr() const noexcept {
    return m_controlBlockRemoteAddr;
}

