

#include <WinSock2.h>
#include "Hook_Socket_TCP.h"
#include "ShellCodeMaker/ShellCode_Socket_TCP_X86.h"
#include "ControlBlockManager/ControlBlock_Socket_TCP.h"

#pragma comment(lib, "ws2_32.lib")

Hook_Socket_TCP::Hook_Socket_TCP(const std::string &ProcessName) : Hook_Socket_TCP(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

Hook_Socket_TCP::Hook_Socket_TCP(DWORD ProcessID) : HookBase(ProcessID) {
    m_controlBlock = std::make_unique<ControlBlock_Socket_TCP>(m_processID);
    m_controlBlock->fillControlBlockArgs();
    m_controlBlock->injectControlBlock();
}

void Hook_Socket_TCP::addHook(std::size_t HookAddr, std::size_t HookLen) {
    auto hookPoint = std::make_unique<HookPoint>(
            m_processID,
            m_controlBlock->getControlBlockRemoteAddr(),
            HookAddr,
            HookLen,
            std::make_unique<ShellCode_Socket_TCP_X86>());

    m_hooks.emplace_back(std::move(hookPoint));
}

void Hook_Socket_TCP::commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) {
    // ��� jmp ��shellcode��ַ
    for (auto &hook: m_hooks) {
        hook->coverCodeJmp();
    }

    m_socketRecvThread = std::thread([FuncRecvData = std::move(FuncRecvData), this]() {
        // WSAStartup
        WSADATA wsaData;

        int iResult;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            return;
        }

        // Create a SOCKET for the server to listen for client connections.
        SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (ListenSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return;
        }

        // Setup the TCP listening socket
        sockaddr_in serAddr{
                AF_INET, //��socket����ʱһ��
                htons(9999),
                INADDR_ANY //�󶨱���������˿�
        };
        iResult = bind(ListenSocket, (sockaddr *) &serAddr, sizeof(serAddr));
        if (iResult == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        // ����������ǰ���һ����Ϣ���գ���������������
        // Accept a client socket
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }
        // No longer need server socket
        closesocket(ListenSocket);

        // Receive until the peer shuts down the connection
        DWORD                       iSendResult = 0;
        std::unique_ptr<DataBuffer> buf(new DataBuffer);
        do {
            iResult = recv(ClientSocket, (char *) buf.get(), sizeof(DataBuffer), 0);
            if (iResult > 0) {

                /// custom function to resolve data
                if (!FuncRecvData(m_processHandle, buf.get())) {
                    break;
                }

                iSendResult = send(ClientSocket, "ReOK", 5, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return;
                }
            } else if (iResult == 0) {
                printf("Connection closing...\n");
            } else {
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return;
            }
        } while (iResult > 0);

        // shutdown the connection since we're done
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

        // cleanup
        closesocket(ClientSocket);
        WSACleanup();
    });
}

void Hook_Socket_TCP::deleteHook(std::size_t HookAddress) {
    for (std::size_t i = 0; i < m_hooks.size(); ++i) {
        if (HookAddress == m_hooks.at(i)->getHookAddr()) {
            m_hooks.at(i)->recoverCodeJmp();
            m_hooks.erase(m_hooks.begin() + i);
            break;
        }
    }
}

/// ����������ǰ���һ����Ϣ���գ�����������accept
Hook_Socket_TCP::~Hook_Socket_TCP() {
    if (m_socketRecvThread.joinable()) {
        /// ��ʽɾ������hook���ֹ�ٴν���Ŀ����̵�socketѭ��
        for (auto   &one: m_hooks) {
            one->recoverCodeJmp();
        }
        /// �ر�Ŀ����̵�socket
        std::size_t closeSocket = m_controlBlock->getControlBlockRef()->PSocketFunctionAddress.closesocket;
        std::size_t wsaCleanup  = m_controlBlock->getControlBlockRef()->PSocketFunctionAddress.WSACleanup;
        auto        hSocket     = Utils::RemoteProcess::readMemory<SOCKET>(
                m_processHandle,
                m_controlBlock->getControlBlockRemoteAddr() + offsetof(ControlBlock, hSocket),
                1
        );

        auto hThread = Utils::AutoPtr::moveHandleOwner(CreateRemoteThread(m_processHandle,
                                                                          nullptr,
                                                                          0,
                                                                          (LPTHREAD_START_ROUTINE) closeSocket,
                                                                          (LPVOID) *hSocket.get(),
                                                                          0,
                                                                          NULL)
        );
        WaitForSingleObject(hThread.get(), INFINITE);

        hThread = Utils::AutoPtr::moveHandleOwner(CreateRemoteThread(m_processHandle,
                                                                     nullptr,
                                                                     0,
                                                                     (LPTHREAD_START_ROUTINE) wsaCleanup,
                                                                     nullptr,
                                                                     0,
                                                                     NULL)
        );
        WaitForSingleObject(hThread.get(), INFINITE);

        m_socketRecvThread.join();
    }
}

