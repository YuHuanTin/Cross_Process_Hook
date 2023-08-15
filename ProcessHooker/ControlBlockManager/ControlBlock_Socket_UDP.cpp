

#include "ControlBlock_Socket_UDP.h"
#include "../../Utils/MyException.h"

void ControlBlock_Socket_UDP::fillControlBlockArgs() {
    // ����Ҳ���ָ�������ĵ�ַ���׳��쳣
    try {
        const char *dlls[] = {"kernelbase.dll", "ws2_32.dll", "ucrtbase.dll"};

        /// kernelbase.dll
        if (!m_rvaReader->isDllLoaded(dlls[0]) || !m_rvaReader->initSearch(dlls[0])) {
            // �����Զ�������ȱ dll ��Ϊ LoadLibraryA �޷���ȡ��
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
        m_controlBlock->PSocketFunctionAddress.sendto      = m_rvaReader->searchRVA("sendto", true).value();
        m_controlBlock->PSocketFunctionAddress.recvfrom    = m_rvaReader->searchRVA("recvfrom", true).value();
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
