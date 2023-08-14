

#include <WinSock2.h>
#include "SocketHook.h"
#include "../ShellCodeMaker/SocketShellCodeX86.h"
#include "../Utils/Utils.h"
#include "../ControlBlockManager/SocketControlBlock.h"

#pragma comment(lib, "ws2_32.lib")

SocketHook::SocketHook(const std::string &ProcessName) : SocketHook(Utils::RemoteProcess::getProcessID(ProcessName).value()) {}

SocketHook::SocketHook(DWORD ProcessID) : ProcessHookBase(ProcessID) {
    m_controlBlock = std::make_unique<SocketControlBlock>(m_processID);
    m_controlBlock->fillControlBlockArgs();
    m_controlBlock->injectControlBlock();
}

void SocketHook::addHook(std::size_t HookAddr, std::size_t HookLen) {
    auto hookPoint = std::make_unique<HookPoint>(
            m_processID,
            m_controlBlock->getControlBlockRemoteAddr(),
            HookAddr,
            HookLen,
            std::make_unique<SocketShellCodeX86>());

    m_hooks.emplace_back(std::move(hookPoint));
}

void SocketHook::commitHook(std::function<bool(HANDLE, DataBuffer *)> FuncRecvData) {
    // 填充 jmp 到shellcode地址
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
                AF_INET, //与socket创建时一样
                htons(9999),
                INADDR_ANY //绑定本机的任意端口
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

        // 至少在析构前完成一次消息接收，否则阻塞在这里
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

void SocketHook::deleteHook(std::size_t HookAddress) {
    for (std::size_t i = 0; i < m_hooks.size(); ++i) {
        if (HookAddress == m_hooks.at(i)->getHookAddr()) {
            m_hooks.at(i)->recoverCodeJmp();
            m_hooks.erase(m_hooks.begin() + i);
            break;
        }
    }
}

/// 至少在析构前完成一次消息接收，否则阻塞在accept
SocketHook::~SocketHook() {
    if (m_socketRecvThread.joinable()) {
        m_socketRecvThread.join();

        // 显式删除所有hook点防止再次进入目标进程的socket循环
        for (auto &one: m_hooks) {
            one->recoverCodeJmp();
        }
        // 关闭目标进程的socket
        std::size_t closeSocket = m_controlBlock->getControlBlockRef()->PSocketFunctionAddress.closesocket;
        std::size_t wsaCleanup  = m_controlBlock->getControlBlockRef()->PSocketFunctionAddress.WSACleanup;
        auto  hSocket     = Utils::RemoteProcess::readMemory<SOCKET>(
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
    }
}

