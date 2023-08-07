

#include "SocketControlBlock.h"
#include "../Utils/Utils.h"
#include "../Utils/MyException.h"

void SocketControlBlock::fillControlBlockArgs() {
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

        m_controlBlock->HookMethod                         = HookMethod::Socket;
        m_controlBlock->PSocketFunctionAddress.WSAStartup  = m_rvaReader->searchRVA("WSAStartup", true).value();
        m_controlBlock->PSocketFunctionAddress.WSACleanup  = m_rvaReader->searchRVA("WSACleanup", true).value();
        m_controlBlock->PSocketFunctionAddress.socket      = m_rvaReader->searchRVA("socket", true).value();
        m_controlBlock->PSocketFunctionAddress.connect     = m_rvaReader->searchRVA("connect", true).value();
        m_controlBlock->PSocketFunctionAddress.send        = m_rvaReader->searchRVA("send", true).value();
        m_controlBlock->PSocketFunctionAddress.recv        = m_rvaReader->searchRVA("recv", true).value();
        m_controlBlock->PSocketFunctionAddress.closesocket = m_rvaReader->searchRVA("closesocket", true).value();
        m_controlBlock->PSocketFunctionAddress.htons       = m_rvaReader->searchRVA("htons", true).value();
        m_controlBlock->PSocketFunctionAddress.inet_addr   = m_rvaReader->searchRVA("inet_addr", true).value();

        /// ucrtbase.dll
        if (!m_rvaReader->isDllLoaded(dlls[2])) {
            Utils::RemoteProcess::loadDll(m_processHandle, m_loadLibraryAddr, dlls[2]);
            m_hookLoadDlls.emplace_back(dlls[2]);
        }
        if (!m_rvaReader->initSearch(dlls[2])) {
            throw MyException("initSearch ucrtbase", __FUNCTION__);
        }

        m_controlBlock->PSocketFunctionAddress.malloc = m_rvaReader->searchRVA("malloc", true).value();
        m_controlBlock->PSocketFunctionAddress.free   = m_rvaReader->searchRVA("free", true).value();
    } catch (std::bad_optional_access &bad_optional_access) {
        throw MyException("can not find function RVA", __FUNCTION__);
    }
}
